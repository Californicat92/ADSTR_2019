#! /bin/sh

### BEGIN INIT INFO
# Provides:          escuchando-para-apagar.py
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
### END INIT INFO


# Realizar funciones específicas cuando el sistema se lo solicite
case "$1" in
  start)
    echo "escuchando-para-apagar.py"
    /usr/local/bin/escuchando-para-apagar.py &
    ;;
  stop)
    echo "escuchando-para-apagar.py"
    pkill -f /usr/local/bin/escuchando-para-apagar.py
    ;;
  *)
    echo "Usage: /etc/init.d/escuchando-para-apagar.sh {start|stop}"
    exit 1
    ;;
esac

exit 0
