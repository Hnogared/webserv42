# Sources #

### RFC (Request For Comments) ###

The http protocol. 
```
https://www.ietf.org/rfc/rfc2616.txt
```

<br/><br/>


# Testing #

## Nginx as example ##

### Step 1 - Nginx ###
```
sudo system nginx start
```

Check if nginx works by loading http://localhost on browser.


### Step 2 - Telnet ###
```
telnet localhost 80
```

If the connection is successful :
```
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
```

Send requests to the nginx server and check the responses.

Example sending a get request `GET / HTTP/1.1`:
```
GET / HTTP/1.1

HTTP/1.1 400 Bad Request
Server: nginx/1.18.0 (Ubuntu)
Date: Sat, 06 Apr 2024 22:14:59 GMT
Content-Type: text/html
Content-Length: 166
Connection: close

<html>
<head><title>400 Bad Request</title></head>
<body>
<center><h1>400 Bad Request</h1></center>
<hr><center>nginx/1.18.0 (Ubuntu)</center>
</body>
</html>
Connection closed by foreign host.
```