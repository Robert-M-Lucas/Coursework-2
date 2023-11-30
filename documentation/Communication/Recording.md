## Starting Recording
#### Controller
1. Sends command to start recording

#### Actor
On receiving command,
1. Clears buffer - set tail to head

## While Recording
#### Controller
Looping through instruments connected:
1. Request new data from Actor ([[Communication Protocol#Receiving Variable-Length Data|Receiving Variable-Length Data]])
2. Passes this data to the Controller Storage

#### Actor
On receiving data request:
1. Replies with data
2. Removes data from buffer

## Ending Recording
#### Controller
1. Sends command to stop recording
2. Completes 