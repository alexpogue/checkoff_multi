// std networking
use std::{
    io::prelude::*, net::{TcpListener, TcpStream}
};
use httparse;
use miniserde::{Serialize, Deserialize, json};
use mysql::*;
use mysql::prelude::*;
use matchit::Router;
use std::str;
use mimalloc::MiMalloc;
use thiserror::Error;

// enable miMalloc memory allocator globally
#[global_allocator]
static GLOBAL: MiMalloc = MiMalloc;

#[derive(Serialize, Debug)]
#[allow(non_snake_case)]
struct TodoItem {
    id: i32,
    title: String,
    details: String,
    isComplete: bool,
}

#[derive(Serialize)]
struct SingleTodoItemResponse {
    status: String,
    data: TodoItem,
}

#[derive(Serialize)]
struct MultipleTodoItemsResponse {
    status: String,
    data: Vec<TodoItem>,
}

#[derive(Serialize)]
struct StatusResponse {
    status: String,
}

#[derive(Deserialize, Debug)]
#[allow(non_snake_case)]
struct TodoItemInsert {
    title: String,
    details: String,
    isComplete: bool,
}

// Error type
type Result<T> = std::result::Result<T, Error>;

// Enumerate possible errors
#[derive(Error, Debug)]
pub enum Error {
    #[error("SQL Query Error")]
    SQLQuery(#[from] mysql::Error), // captures preceeding mysql error

    #[error("Invalid input Parameter - failed to dederialize JSON")]
    ParameterInvalidJson(#[from] miniserde::Error), // captures preceeding miniserde error

    #[error("Invalid input id parameter - failed to parse integer")]
    ParameterInvalidId(#[from] std::num::ParseIntError), // captures preceeding error

    #[error("Missing input id parameter")]
    ParameterMissingId(String), // captures preceeding error

    #[error("Todo item not found for id {0}")]
    TodoItemNotFound(i32),

    #[error("http route error - can't find {0}")]
    HttpRoute(String),
}

// Define the get_users function as before
fn get_todo_items(pool: &mut PooledConn) -> Result<String> {
    #[allow(non_snake_case)]
    let todo_items = pool.query_map("SELECT id, title, details, isComplete FROM todo_item",
        |(id, title, details, isComplete) | {
            TodoItem {id, title, details, isComplete}
        }
    ).map_err(Error::SQLQuery)?;

    let todo_items_response = MultipleTodoItemsResponse {
        status: "success".to_string(),
        data: todo_items,
    };

    Ok(json::to_string(&todo_items_response))
}

fn create_todo_item(pool: &mut PooledConn, todo_item: &str) -> Result<String> {
    // parse todo item
    let todo_item: TodoItemInsert = json::from_str(todo_item)
        .map_err(Error::ParameterInvalidJson)?;


    pool.exec_drop(r"INSERT INTO todo_item (title, details, isComplete) VALUES (?, ?, ?)",
        (&todo_item.title, &todo_item.details, &todo_item.isComplete),
    ).map_err(Error::SQLQuery)?;

    Ok(json::to_string(&StatusResponse {status: "success".to_string()}))
}

fn get_todo_item_helper(id: i32, pool: &mut PooledConn) -> Result<Option<TodoItem>> {
    let result = pool.exec_first("SELECT id, title, details, isComplete FROM todo_item WHERE id=?",(id,))
        .map_err(Error::SQLQuery)?;
    #[allow(non_snake_case)]
    let todo_item_option = result.map(|(id, title, details, isComplete) | {
        TodoItem{ id, title, details, isComplete}
    });

    return Ok(todo_item_option)
}

fn get_todo_item(pool: &mut PooledConn, id: Option<&str>) -> Result<String> {
    let id: i32 = id
        .ok_or(Error::ParameterMissingId("Missing id".into()))
        .and_then(|s| s.parse::<i32>()
            .map_err(Error::ParameterInvalidId)
        )?;

    let todo_item = get_todo_item_helper(id, pool)?
        .ok_or(Error::TodoItemNotFound(id))?;

    let todo_item_response = SingleTodoItemResponse {
        status: "success".to_string(),
        data: todo_item,
    };

    Ok(json::to_string(&todo_item_response))
}


fn update_todo_item(pool: &mut PooledConn, id: Option<&str>, todo_item: &str) -> Result<String> {
    let id: i32 = id.unwrap().parse::<i32>()
        .map_err(Error::ParameterInvalidId)?; 

    dbg!(&todo_item);
    let todo_item: TodoItemInsert = json::from_str(todo_item)
        .map_err(Error::ParameterInvalidJson)?;

    let _existing_todo_item = get_todo_item_helper(id, pool)?
        .ok_or(Error::TodoItemNotFound(id))?;

    pool.exec_drop(r"UPDATE todo_item SET title = ?, details = ?, isComplete = ? WHERE id = ?",
        (todo_item.title, todo_item.details,&todo_item.isComplete, id),
    ).map_err(Error::SQLQuery)?;

    Ok(json::to_string(&StatusResponse {status: "success".to_string()}))
}

fn delete_todo_item(pool: &mut PooledConn, id: Option<&str>) -> Result<String> {
    let id: i32 = id.unwrap().parse::<i32>()
        .map_err(Error::ParameterInvalidId)?; 
    let _existing_todo_item = get_todo_item_helper(id, pool)?
        .ok_or(Error::TodoItemNotFound(id))?;

    pool.exec_drop(r"DELETE FROM todo_item WHERE id = ?", (id,))
        .map_err(Error::SQLQuery)?;

    Ok(json::to_string(&StatusResponse {status: "success".to_string()}))
}

fn main() {
    // Set up the database connection pool
    // docker database
    // let database_url = "mysql://root:strong_password@checkoff-mysql:3306/checkoff";
    // local database
    let database_url = "mysql://root:strong_password@127.0.0.1:3307/checkoff";
    // connect to sql database
    let pool = Pool::new(database_url)
        .expect("Could not connect to the database");
    //TODO rename pool to conn
    let mut pooled_conn = pool.get_conn().unwrap();

    // tcp server
    let listener = TcpListener::bind("0.0.0.0:3000").unwrap();

    for stream in listener.incoming() {
        let stream = stream.unwrap();
        handle_connection(stream, &mut pooled_conn).unwrap();
    }
}

fn handle_connection(mut stream: TcpStream, pool: &mut PooledConn) -> std::result::Result<(), Box<dyn std::error::Error>> {
    // read request into fixed size buffer
    let mut buf = [0; 1024];
    let bytes_read = stream.read(&mut buf).unwrap(); //FUTURE check full stream was read

    // parse headers
    let mut headers = [httparse::EMPTY_HEADER; 16];
    let mut request = httparse::Request::new(&mut headers);    
    let header_len = match request.parse(&buf[..bytes_read])? {
        httparse::Status::Complete(l) => l,
        httparse::Status::Partial => return Err("Incomplete HTTP request".into()),
    };

    let path = request.path.unwrap_or("");
    let method = request.method.unwrap_or("");

    // Extract the body, which is the data after the headers.
    let body_bytes = &buf[header_len..bytes_read];
    let body = str::from_utf8(body_bytes).unwrap_or("");

    // Create the router
    //future: do not re-construct router for each request
    let mut router = Router::new();
    router.insert("/todo-item", "/todo_item")?;
    router.insert("/todo-item/{id}", "/todo-item/{id}")?;
    let matched = router.at(path)?;
    let matched_route = *matched.value;
    dbg!(matched_route);
    dbg!(method);
    
    
    let response = match (matched_route, method) {
        ("/todo_item", "POST") => create_todo_item(pool, body),
        ("/todo_item", "GET") => get_todo_items(pool),
        ("/todo-item/{id}", "GET") => get_todo_item(pool, matched.params.get("id")),
        ("/todo-item/{id}", "PUT") => update_todo_item(pool, matched.params.get("id"), body),
        ("/todo-item/{id}", "DELETE") => delete_todo_item(pool, matched.params.get("id")),
        _ => Err(Error::HttpRoute(String::from(matched_route))),
    };

    let (response_status, response) = match response {
        Ok(res) => ("HTTP/1.1 200 OK", res),
        Err(e) => ("HTTP/1.1 500 Internal Server Error", e.to_string()),
    };

    let response_length = response.len();
    let response = format!("{response_status}\r\nContent-Length: {response_length}\r\n\r\n{response}");
    stream.write_all(response.as_bytes()).unwrap();
    
    Ok(())
}
