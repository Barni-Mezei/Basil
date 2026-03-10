# What is **Basil**
**Basil** is a basic minecraft server written in `C` and `LUA`
It is easily moddable, since the server and game logic is implemented in `LUA`
**IMPORTANT: This server was written on and tested on Arch linux only***


<div align="center">
  <img src="server-icon.png" alt="Server icon" width="100">
</div>

# Why was it created
It's purpose it to provide a **fast** and easy-to-mod server for everybody to use.
It **lacks many fatures of minecraft**, but instead focuses on building, specifically **making megabuilds**

# How to use
You have to run the `main` compiled binary and the server should start on your machine. For other people to connect they either need to be onthe same network with you, or you have to open a port for other people to connect to your machine. (Help to opening ports: https://www.wikihow.com/Open-Ports). **Be careful when opening ports to the internet, they might be dangerous!**

Before starting the script you can configure the server using a `server.properties` text file, where in each line there is a config option name, then an equals sign (=) and the the respective config value for that option.

It looks something like this:
```properties
max-players=4
modt=My awesome server!
```

For the list of all available config options read the [server config](doc/server_config)

# How to join
If server was started then it will act as any other minecraft server, meaning that by knowing it's **IP** and **PORT** (default is 25565) you can connect to it, with any minecraft client running the same version of minecraft as the server.

# Modding
Modding the server os very easy. You just havce to write some LUA functions and a config file for your mod, thenplace all this insides the `mods/your_mod_name/` folder.
See the [modding quide](doc/modding_guide.md) for more information.