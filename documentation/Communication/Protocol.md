#### Notes
- All I2C communications must be requested by the Controller
- When the controller requests data, it must request a fixed amount of bytes and this amount is not visible to the Actor
- When sending data, any amount of bytes can be sent
- When connected the Controller and Actors ('Instruments') are expected to behave as one circuit so no consideration is given to error checking/correction

>[!warning] 
>Sending or requesting data calls an interrupt on the recipient. If a second interrupt is called while the first is still being resolved the second interrupt will be ignored leading to undefined behaviour! Use `delay(TRANSMISSION_DELAY)` between any two sequential transfers/requests to prevent errors.
## Instrument Detection: TODO
## Sending Commands
Sending a command from Controller → Actor
1. `Wire.beginTransfer` is called on the Controller
2. A single byte (from the enum `Code`) representing the command is sent to the Actor
3. `Wire.endTransfer` is called on the Controller
## Sending Data
Sending data from Controller → Actor
1. `Wire.beginTransfer` is called on the Controller
2. A single byte (from the enum `Code`) representing the data type is sent to the Actor
3. The data is transfered
4. `Wire.endTransfer` is called on the Controller

## Requesting Data
Requesting data from Actor → Controller
1. A command ([[#Sending Commands]]) is sent to request a data type from the Actor
2. The actor stores this request type so that it knows what data to reply with on the next data request interrupt

## Receiving Variable-Length Data
Receiving data from Actor → Controller
1. The length of the data available is requested ([[#Requesting Data]]). This comes through as a fixed-length (currently 1 byte) unsigned integer. This length is also cached on the Actor side - see warning below for reason
2. The data is requested ([[#Requesting Data]])

>[!warning] 
>When the data length is requested, the returned value must be cached to prevent too many bytes from being sent if data length changes between the two requests

>[!Info] 
>This two-step process is necessary as, unlike the Actor which can keep reading bytes until the transfer is finished, the Controller must specify the amount of bytes it is requesting from the Actor.
