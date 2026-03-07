# What is **Basil**
**Basil** is a basic minecraft server written in `LUA`

<div align="center">
  <img src="server-icon.png" alt="Server icon" width="100">
</div>

# Why was it created
It's purpose it to provide a **fast** and easy-to-mod server for everybody to use.
It **lacks many fatures of minecraft**, but instead focuses on building, specifically **making megabuilds**

# How to use
You have to run the `main.lua` script and the server should start on your machine. For other people to connect they either need to be onthe same network with you, or you have to open a port for other people to connect to your machine. (Help to opening ports: https://www.wikihow.com/Open-Ports). **Be careful when opening ports to the internet, they might be dangerous!**

Before starting the script you can configure the server using a `server.properties` text file, where in each line there is a config option name, then an equals sign (=) and the the respective config value for that option.

It looks something like this:
```properties
max-players=4
modt=My awesome server!
```

For the list of all available config options read the [server config](doc/server_config)

# How to join
If server was started then it will act as any other minecraft server, meaning that by knowing it's **IP** and **PORT** (default is 25565) you can connect to it, with any minecraft client running the same version of minecraft as the server.