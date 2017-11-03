# #include "sysnum.h"
# #include "sys_handlers.h"



# .GLOBAL _sys_call_int
# _sys_call_int:
# 	cmpl $SYS_HALT, %EAX
# 	je halt
# 	cmpl $SYS_EXECUTE, %EAX
# 	je execute
# 	cmpl $SYS_READ, %EAX
# 	je read
# 	cmpl $SYS_WRITE, %EAX
# 	je write
# 	cmpl $SYS_OPEN, %EAX
# 	je open
# 	cmpl $SYS_CLOSE, %EAX
# 	je close
# 	cmpl $SYS_GETARGS, %EAX
# 	je getargs
# 	cmpl $SYS_VIDMAP, %EAX
# 	je vidmap
# 	cmpl $SYS_SET_HANDLER, %EAX
# 	je set_handler
# 	cmpl $SYS_SIGRETURN, %EAX
# 	je sigreturn









