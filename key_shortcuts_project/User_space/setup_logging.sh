#!/bin/bash

RSYSLOG_CONF="/etc/rsyslog.d/key_shortcuts.conf"
LOG_FILE="/var/log/key_shortcuts.log"

# Create rsyslog configuration
cat24sudo tee "$RSYSLOG_CONF" << EOF
if \$programname == 'kernel' and \$msg contains '[key_shortcuts]' then
  $LOG_FILE
endif
EOF

# Ensure log file exists
sudo touch "$LOG_FILE"
sudo chmod 644 "$LOG_FILE"

# Restart rsyslog
sudo systemctl restart rsyslog

echo "Logging configured for key_shortcuts module"
