import Tkinter

main = Tkinter.Tk()
main.withdraw()

splash = Tkinter.Toplevel()
splash.overrideredirect(1)
# Add the image to display to the splash window.

doAppLoading(main)

splash.destroy()
main.deiconify()
main.mainloop()
