## Starting Recording
#### Controller
1. Sends command to start recording

#### Actor
On receiving command,
1. Clears buffer - set tail to head

## While Recording
#### Controller
Looping through instruments connected:
1. Request new data from Actor ([[Protocol#Receiving Variable-Length Data|Receiving Variable-Length Data]])
2. Passes this data to the Controller Storage for persistent storage

#### Actor
On receiving data request:
1. Replies with data
2. Removes data from buffer

## Ending Recording
#### Controller
1. Sends command to stop recording
2. Completes final 'While Recording' loop

>[!info] 
>Stop recording command is issued before final loop to ensure all instruments cut off their last note at near the same time.

