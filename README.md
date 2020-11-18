# wifi-scheduler

Minimal wifi scheduler that's been tested on macOSX (Catalina 10.15.7) and Lubuntu (Ubuntu 18.04.5 LTS). 

### To build
```
$ make all
```

### To set "wifi off" time
```
$ ./swifi.out add -s <start_time> -e <end_time>
```
where `<start_time>` and `<end_time>` are in the form `HHMM`. Wifi will shut off at `<start_time>` and turn back on again at `<end_time>`. 

### To remove "wifi off" time
```
$ ./swifi.out rmv -s <start_time> -e <end_time>
```

### To see set times
```
$ crontab -l
```

### To reset
```
$ ./swifi.out clear
```
