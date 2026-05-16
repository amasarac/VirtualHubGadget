import subprocess
import tkinter as tk
from tkinter import messagebox

def list_devices():
    """Returns a list of device names from usbip list --local."""
    try:
        output = subprocess.check_output(['usbip', 'list', '--local']).decode('utf-8')
        device_names = []
        for line in output.split('\n'):
            if 'busid' in line.lower() or 'bus=' in line.lower():
                # Try to parse busid
                parts = line.split('=')
                if len(parts) > 1:
                    device_name = parts[-1].strip().split()[0]
                    device_names.append(device_name)
        return device_names
    except (subprocess.CalledProcessError, FileNotFoundError):
        return []

def bind_device(device_name):
    """Binds the specified device using usbip."""
    subprocess.check_call(['sudo', 'usbip', 'bind', '-b', device_name])

def unbind_device(device_name):
    """Unbinds the specified device using usbip."""
    subprocess.check_call(['sudo', 'usbip', 'unbind', '-b', device_name])

class App(tk.Tk):
    """Main Application Window."""
    def __init__(self):
        super().__init__()
        self.title("USB/IP Bindings")
        self.geometry("400x400")

        self.device_states = {}

        self.device_listbox = tk.Listbox(self)
        self.device_listbox.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        # Create a button to bind the selected device
        self.bind_button = tk.Button(self, text='Bind', command=self.gui_bind_device)
        self.bind_button.pack(side=tk.TOP, fill=tk.X)

        # Create a button to unbind the selected device
        self.unbind_button = tk.Button(self, text='Unbind', command=self.gui_unbind_device)
        self.unbind_button.pack(side=tk.TOP, fill=tk.X)

        self.update_device_listbox()

    def update_device_listbox(self):
        """Periodically update the list of devices."""
        device_names = list_devices()

        # Update states
        current_keys = list(self.device_states.keys())
        for dev in current_keys:
            if dev not in device_names:
                del self.device_states[dev]

        for dev in device_names:
            if dev not in self.device_states:
                self.device_states[dev] = False

        self.device_listbox.delete(0, tk.END)

        for dev, state in self.device_states.items():
            status = 'Bound' if state else 'Unbound'
            self.device_listbox.insert(tk.END, f"{dev} ({status})")
            # Set background color using itemconfig
            idx = self.device_listbox.size() - 1
            if state:
                self.device_listbox.itemconfig(idx, {'bg': 'green'})
            else:
                self.device_listbox.itemconfig(idx, {'bg': 'red'})

        self.after(2000, self.update_device_listbox)

    def gui_bind_device(self):
        """Bind the currently selected device."""
        selection = self.device_listbox.curselection()
        if not selection:
            return

        item = self.device_listbox.get(selection[0])
        device_name = item.split(' ')[0]

        try:
            bind_device(device_name)
            self.device_states[device_name] = True
            self.update_device_listbox()
        except subprocess.CalledProcessError as e:
            messagebox.showerror("Error", f"Failed to bind {device_name}:\n{e}")

    def gui_unbind_device(self):
        """Unbind the currently selected device."""
        selection = self.device_listbox.curselection()
        if not selection:
            return

        item = self.device_listbox.get(selection[0])
        device_name = item.split(' ')[0]

        try:
            unbind_device(device_name)
            self.device_states[device_name] = False
            self.update_device_listbox()
        except subprocess.CalledProcessError as e:
            messagebox.showerror("Error", f"Failed to unbind {device_name}:\n{e}")

if __name__ == "__main__":
    app = App()
    app.mainloop()
