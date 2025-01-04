# WebSocket4Fragment
THe only WebSocket solution to handle fragment dynamically, the first time ever!<p>
This ultimate echo server can satisfy your every need of WebSocket development! The flexible asynchronous client can also be used for any WebSocket server supporting ws/wss and http/https schemas!

<h2>Pre-requiste</h2>
WebSocket feature for IIS need be enabled<p><p>
<img src="https://github.com/user-attachments/assets/3546c0ea-18d4-4e6e-968e-52802669da1a" width=40%>

<h2>Installation</h2>
Download all files from server folder to your desired one. Launch a command line with administrator privilege and enter to your download folder. Run command "install.bat userAccount userPassword" to complete installation. userAccount/userPassword need be your own Windows credential and make sure that it has administrator privilege. This credential is only used by IIS in configuring a new app pool and not used by this solution or stored anywhere else.<p><p>

To verify, launch browser (Chrome/Safari/Edge) and enter URL "http://localhost/InteractWS/websocket.html"<p>
<img src="https://github.com/user-attachments/assets/6b36954b-4aff-49d2-92f0-aed42aa95e1f" width=70%>

<h2>Uninstallation</h2>
Run command "uninstall.bat" in command line with administrator privilege in your download folder.

<h2>Accepted setting in server</h2><p>
1. "Mode: UTF8/Binary" - Set to use UTF8 or Binary mode in server response<p>
2. "Fragment: numeric value" - A greater than 0 integer indicates a fragment size to be used in server response. 0 means no fragmentation in server response<p>
3. "help" or "?" - Display all supported commands and their usages<p>
 
<h2>Client sample code output</h2>
Synchronous client sample codes output<p><p><p>
<img src="https://github.com/user-attachments/assets/b58c56a9-454f-41d7-89fb-10832397ac9a" width=65%>
<img src="https://github.com/user-attachments/assets/d79eee8e-1c54-4809-9ccf-ce3612d15bfb" width=65%>
<img src="https://github.com/user-attachments/assets/e43e4844-fe64-466c-be9a-f92d15fae63a" width=65%>

Asynchronous client output<p><p>
<img src="https://github.com/user-attachments/assets/93c2b521-80bd-45a0-be6e-2757de979720" width=70%>

<h2>Consulting service</h2>
1. WebSocket design or implementation in either server or client<p>
2. Convert existing Windows app to any device through WebSocket as a new transportation layer<p>

<h2>Related contribution</h2>
<a href="https://github.com/duduyoyo/WebSocket4OPC">WebSocket4OPC</a>, the modern way to retrieve classic OPC data!<p>
<a href="https://github.com/duduyoyo/OLEDB4OPC">OLEDB4OPC</a>, the fastest way to transfer OPC data to database!
  
