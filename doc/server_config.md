# What is server.properties
For the server configuration you have to place a file named `server.properties` next to the script. After doing this, you can use this file to configure the server.

# File structure
The structure of this files is the following:
- Comments  are full line only and are prefixed with a `#` character
- Empty lines are allowed
- Config options are as follows:
  - The name of the property you want to modify
  - An equals sign (=)
  - Then, the value of the property

**No whitespace is allowed before, or after the equals sign!**

# Available properties
Some of them are from an official minecraft server, but there are some custom ones.

## Official properties

### max-players
#### Description
Limits the maximum amount of players who can join the server
#### Example
```
max-players=20
```

### modt
#### Description
Sets the description of your server in the server browser menu. An excelleln modt creator can be found [here](https://mctools.org/motd-creator) (use the "official/vanilla" version)
#### Example
```
modt=\u00A74My awesome server!
```

### server-port
#### Description
Sets the port of your server. (Where the seerver will listen for incoming packets) The default value is **25565** If you leave it on default clients dont have to specify the port when connecting to it.
#### Example
```
server-port=25565
```

## Custom basil properties

### World type
#### Description
The type of the generated world, values can be:
- stone: For a 1 layer superflat stone world
- default: For a 5 layer tall: grass_bock, dirt, stone, stone, bedrock superflat
- noise: For a perlin noise random terrain generation
#### Example
```
world-type=noise
```

### World size
#### Description
Sets the maximum size of a minecraft world. This property specifies the side length of the rectangle (in chunks), which is the world boder it self
#### Example
```
world-size=32
```