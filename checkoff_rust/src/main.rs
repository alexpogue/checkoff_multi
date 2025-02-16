// std networking
use std::{
    io::prelude::*,
    net::{TcpListener, TcpStream},
};
use httparse;

// ----------------------------------
use serde::{Serialize, Deserialize};
use serde_json::json;
use sqlx::{MySqlPool, Row};
use tokio::runtime::Runtime;
use matchit::Router;
use std::str;
// use mimalloc::MiMalloc;

// #[global_allocator]
// static GLOBAL: MiMalloc = MiMalloc;

#[derive(Serialize)]
#[derive(Debug)]
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

#[derive(Deserialize)]
#[derive(Debug)]
#[allow(non_snake_case)]
struct TodoItemInsert {
    title: String,
    details: String,
    isComplete: bool,
}

// Define the get_users function as before
fn get_todo_items(pool: &MySqlPool) -> String {
    let rows_fut = sqlx::query("SELECT id, title, details, isComplete FROM todo_item").fetch_all(pool);
    let rt = Runtime::new().unwrap();
    let rows = rt.block_on(rows_fut).unwrap();

    let todo_items: Vec<TodoItem> = rows
        .into_iter()
        .map(|row| {
            TodoItem {
                id: row.try_get::<i32, _>("id").unwrap_or_default(),
                title: row.try_get::<String, _>("title").unwrap_or_default(),
                details: row.try_get::<String, _>("details").unwrap_or_default(),
                isComplete: row.try_get::<bool, _>("isComplete").unwrap_or_default(),
            }
        })
        .collect();

    let todo_items_response = MultipleTodoItemsResponse {
        status: "success".to_string(),
        data: todo_items,
    };

    json!(todo_items_response).to_string()
}

fn create_todo_item(pool: &MySqlPool, todo_item: &str) -> String {
    // parse todo item
    let todo_item: TodoItemInsert = serde_json::from_str(todo_item).expect("Invalid JSON");

    let add_item_fut = sqlx::query("INSERT INTO todo_item (title, details, isComplete) VALUES (?, ?, ?)")
        .bind(&todo_item.title)
        .bind(&todo_item.details)
        .bind(&todo_item.isComplete)
        .fetch_all(pool);
    let rt = Runtime::new().unwrap();
    rt.block_on(add_item_fut).unwrap();

    json!(StatusResponse {status: "success".to_string()}).to_string()
}

fn get_todo_item_helper(id: i32, pool: &MySqlPool) -> Result<Option<TodoItem>, String> {
    let row_fut = sqlx::query("SELECT id, title, details, isComplete FROM todo_item WHERE id=?")
        .bind(id)
        .fetch_all(pool);
    let rt = Runtime::new().unwrap();
    let row = rt.block_on(row_fut).unwrap();

    let todo_item_option: Option<TodoItem> = row
        .into_iter()
        .map(|row| {
            TodoItem {
                id: row.try_get::<i32, _>("id").unwrap_or_default(),
                title: row.try_get::<String, _>("title").unwrap_or_default(),
                details: row.try_get::<String, _>("details").unwrap_or_default(),
                isComplete: row.try_get::<bool, _>("isComplete").unwrap_or_default(),
            }
        })
        .collect::<Vec<TodoItem>>()
        .pop();

    return Ok(todo_item_option)
}

fn get_todo_item(pool: &MySqlPool, id: Option<&str>) -> String {
    let id: i32 = id.unwrap().parse::<i32>().unwrap(); 
    let todo_item = match get_todo_item_helper(id, &pool) {
        Ok(Some(todo_item)) => todo_item,
        Ok(None) => {
            eprintln!("Application error: could not find todo item with id {id}");
            return String::from("Not found")

        },
        Err(e) => {
            eprintln!("Application error occurred getting item with id {id}. Error: {e}");
            return String::from("Internal server error");
        }
    };
    println!("todo_item = {:?}", todo_item);
    let todo_item_response = SingleTodoItemResponse {
        status: "success".to_string(),
        data: todo_item,
    };

    json!(todo_item_response).to_string()
}


fn update_todo_item(pool: &MySqlPool, id: Option<&str>, todo_item: &str) -> String {
    let id: i32 = id.unwrap().parse::<i32>().unwrap(); 
    let todo_item: TodoItemInsert = serde_json::from_str(todo_item).expect("Invalid JSON");

    let existing_todo_item = match get_todo_item_helper(id, &pool) {
        Ok(Some(todo)) => todo,
        Ok(None) => {
            panic!("not found");
        }
        Err(e) => {
            eprintln!("Application error occurred getting item with id {id}. Error: {e}");
            panic!("Internal server error")
        }
    };
    println!("existing todo_item = {:?}", existing_todo_item);

    let update_fut =  sqlx::query("UPDATE todo_item SET title = ?, details = ?, isComplete = ? WHERE id = ?")
        .bind(todo_item.title)
        .bind(todo_item.details)
        .bind(todo_item.isComplete)
        .bind(id)
        .fetch_all(pool);
    let rt = Runtime::new().unwrap();
    rt.block_on(update_fut).unwrap();

    json!(StatusResponse {status: "success".to_string()}).to_string()
}

fn delete_todo_item(pool: &MySqlPool, id: Option<&str>) -> String {
    let id: i32 = id.unwrap().parse::<i32>().unwrap(); 
    let existing_todo_item = match get_todo_item_helper(id, &pool) {
        Ok(Some(todo)) => todo,
        Ok(None) =>
            panic!("Not found"),
        Err(e) => {
            eprintln!("Application error occurred getting item with id {id}. Error: {e}");
            panic!("Internal server error")
        }

    };
    println!("deleting todo_item = {:?}", existing_todo_item);

    let delete_fut = sqlx::query("DELETE FROM todo_item WHERE id = ?")
        .bind(id)
        .fetch_all(pool);
    let rt = Runtime::new().unwrap();
    rt.block_on(delete_fut).unwrap();

    json!(StatusResponse {status: "success".to_string()}).to_string()
}

fn main() {
    // Set up the database connection pool
    // docker database
    // let database_url = "mysql://root:strong_password@checkoff-mysql:3306/checkoff";
    // local database
    let database_url = "mysql://root:strong_password@127.0.0.1:3307/checkoff";
    let rt = Runtime::new().unwrap();
    // connect to sql database
    let pool = rt.block_on(MySqlPool::connect(&database_url))
        .expect("Could not connect to the database");

    // tcp server
    let listener = TcpListener::bind("0.0.0.0:3000").unwrap();

    for stream in listener.incoming() {
        let stream = stream.unwrap();
        handle_connection(stream, &pool).unwrap();
    }
}

fn handle_connection(mut stream: TcpStream,pool: &MySqlPool) -> Result<(), Box<dyn std::error::Error>> {
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
        ("/todo_item", "POST") => create_todo_item(&pool, body),
        ("/todo_item", "GET") => get_todo_items(&pool),
        ("/todo-item/{id}", "GET") => get_todo_item(&pool, matched.params.get("id")),
        ("/todo-item/{id}", "PUT") => update_todo_item(&pool, matched.params.get("id"), body),
        ("/todo-item/{id}", "DELETE") => delete_todo_item(&pool, matched.params.get("id")),

        _ => todo!("no matching route"),
    };

    let response_status =  "HTTP/1.1 200 OK";
    let response_length = response.len();
    let response = format!("{response_status}\r\nContent-Length: {response_length}\r\n\r\n{response}");
    stream.write_all(response.as_bytes()).unwrap();
    
    Ok(())
}
