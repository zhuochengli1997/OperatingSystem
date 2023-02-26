#Unix Shell Program

**This is a simple Unix shell program written in C language.**

##Features
- Displays the current working directory along with the username in the prompt.

- Supports changing the text color of the prompt.

##Usage

```make```

```./shell```

##Changing the text color

To change the text color of the prompt, you can use the following escape codes:

- *30m - Black*
- *31m - Red*
- *32m - Green*
- *33m - Yellow*
- *34m - Blue*
- *35m - Purple*
- *36m - Cyan*
- *37m - White*

For example, to change the prompt color to cyan, you can use the following command:

```
PS1='\[\e[1;36m\][\u@\h \w]\$ \[\e[0m\]'
```