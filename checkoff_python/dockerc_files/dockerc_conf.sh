# define ZSCALER_CERT_DIR in ~/.agps_conf
# located locally so that setting doesn't get pushed to the repo
if [ -f "${HOME}/.agps_conf" ]; then
  . "${HOME}/.agps_conf"
fi
