### viewimage.py
from Tkinter import *

root = Tk()
root.overrideredirect(1)
root.geometry('+100+100')
frame = Frame(root, colormap="new", visual='truecolor').pack()
imagedata = PhotoImage(file='/usr/share/texi2html/images/contents_motif.gif')
# imagedata = PhotoImage(file='/usr/share/texi2html/images/xy_next.gif')
label = Label(frame, image=imagedata).pack()
#quitbutton = Button(root, text="Quit", command=root.destroy).pack(fill=X)
root.mainloop()
