alias pish='__(){ for l in `cat test.sh`;do sudo sh -c "echo $l | tr -s \"\n\" \"\r\" > /dev/ttyUSB0;sleep 0.01;";  done } ; __'
