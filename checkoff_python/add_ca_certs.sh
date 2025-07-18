#!/usr/bin/env bash

CERTS_DIR="custom_certs"
#INSTALL_DIR="/etc/ssl/certs"
INSTALL_DIR="/usr/local/share/ca-certificates"
AGGREGATE_CERT_FILE="agps_api_certs.crt"

cert_files="$(find "$CERTS_DIR" -type f -name "*.crt")"

# only add certs when we found one or more crt files
if [ "$(echo "$cert_files" | grep -vc '^\s*$')" -ge 1 ]; then
  cat $cert_files > "$AGGREGATE_CERT_FILE"
  cp "$AGGREGATE_CERT_FILE" "${INSTALL_DIR}/"
else
  echo "No custom certs found in $CERTS_DIR. Skipping cert install."
fi
