# EDF Schedular
# <center>Thesis Implementation</center>
---
### 1. Define Macro
- In FreeRTOSConfig.h

![Screenshot](screenshots/Pasted%20image%2020220923234008.png)
---
### 2. Define The New EDF List
- In tasks.c

![Screenshot](screenshots/Pasted%20image%2020220923221401.png)
---
### 3. Initialize The New EDF List
- In tasks.c in `prvInitialiseTaskLists()`

![Screenshot](screenshots/Pasted%20image%2020220923221925.png)
---
### 4. Modify The Method That Adds A Task To The Ready List
- In tasks.c

![Screenshot](screenshots/Pasted%20image%2020220923224300.png)
- Note: When adding a new task using `vListInsert()` function it inserts this new `xStateListItem` node in the `xReadyTasksListEDF` list at a position according to the value inside the member variable `xStateListItem.xItemValue` in such a way so that the nodes inside the list are sorted in ascending order according to this value. So We should make `xItemValue` of each task node hold the task deadline
---
### 5. Modify TCB Struct
- In tasks.c

![Screenshot](screenshots/Pasted%20image%2020220923224935.png)
---
### 6. Create A New Task Initialization Method
- In tasks.c

![Screenshot](screenshots/Pasted%20image%2020220923231453.png)
![Screenshot](screenshots/Pasted%20image%2020220923231417.png)
---
### 7. Modify Initialization of IDLE Task
- In tasks.c in `vTaskStartScheduler()`

![Screenshot](screenshots/Pasted%20image%2020220923231857.png)
- Note: We will have to make sure that the IDLE tasks stays at the end of the EDF list. This is just initialization if we didn't do anything else when the system starts running for a while the IDLE task will eventually preempt other application tasks.
> Every time IDLE task executes (i.e. no other tasks are in the Ready List), it calls a method that increments its deadline in order to guarantee that IDLE task will remain in the last position of the Ready List.
---
### 8. Choose The Task At The Head Of The EDF List When Context Switching
- In tasks.c in `vTaskSwitchContext()`

![Screenshot](screenshots/Pasted%20image%2020220923233123.png)
- Note:  All That this function `vTaskSwitchContext()` does is it selects the task that will run and assigns it to `pxCrruntTCB`
