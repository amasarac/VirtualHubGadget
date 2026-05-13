import subprocess
import tkinter as tk

# Global variables used by the GUI logic
devices = []
checked_items = []
bound_devices = []

def list_devices():
    # Execute the 'list --local' command and capture the output
    output = subprocess.check_output(['usbip', 'list', '--local']).decode('utf-8')
    
    # Extract the device names from the output
    device_names = []
    for line in output.split('\n'):
        if 'Bus=' in line:
            device_name = line.split(' ')[-1].strip()
            device_names.append(device_name)
    
    return device_names

def bind_device(device_name):
    # Execute the 'bind' command for the specified device
    subprocess.check_call(['sudo', 'usbip', 'bind', '-b', device_name])

def unbind_device(device_name):
    # Execute the 'unbind' command for the specified device
    subprocess.check_call(['sudo', 'usbip', 'unbind', '-b', device_name])

class App(tk.Tk):
    def __init__(self):
        super().__init__()
        
        # Create a dictionary to store the device names and their state (bound/unbound)
        self.device_states = {}
        
        # Create a listbox to display the device names
        self.device_listbox = tk.Listbox(self)
        self.device_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # Create a button to bind the selected device
        self.bind_button = tk.Button(self, text='Bind', command=self.bind_device)
        self.bind_button.pack(side=tk.TOP, fill=tk.X)
        
        # Create a button to unbind the selected device
        self.unbind_button = tk.Button(self, text='Unbind', command=self.unbind_device)
        self.unbind_button.pack(side=tk.TOP, fill=tk.X)
        
        # Populate the device listbox
        self.update_device_listbox()
        
    def update_device_listbox(self):
        # Get the list of device names
        device_names = list_devices()
        
        # Remove any devices that are no longer connected
        for device_name in list(self.device_states.keys()):
            if device_name not in device_names:
                del self.device_states[device_name]
        
        # Add any new devices that have been connected
        for device_name in device_names:
            if device_name not in self.device_states:
                self.device_states[device_name] = False
        
        # Clear the device listbox
        self.device_listbox.delete(0, tk.END)
        
        # Populate the device listbox with the device names and their state
        for device_name, state in self.device_states.items():
            if state:
                self.device_listbox.insert(tk.END, device_name + ' (Bound)', 'bound')
            else:
                self.device_listbox.insert(tk.END, device_name + ' (Unbound)', 'unbound')
        
        # Set the background color of the device listbox items based on their state
        self.device_listbox.tag_configure('bound', background='green')
        self.device_listbox.tag_configure('unbound', background='red')
        
        # Schedule the next update after 1 second
        self.after(1000, self.update_device_listbox)
    
    def bind_device(self):
        # Get the selected device name from the device listbox
        selection = self.device_listbox.curselection()
        if len(selection) != 1:
            return
        
        device_name = self.device_listbox.get(selection[0]).split(' ')[0]
        
        # Bind the selected device
        selected_devices = [devices[i] for i in checked_items]
        for device in selected_devices:
            device_name = device.split()[0]
            print(f"Binding {device_name}...")
            subprocess.run([
                "sudo",
                "usbip",
                "bind",
                "-b",
                device_name,
            ], check=True)
            bound_devices.append(device_name)
            
        # Update the checkboxes
        for i, device in enumerate(devices):
            device_name = device.split()[0]
            if device_name in bound_devices:
                device_checkboxes[i].config(fg="green")
            else:
                device_checkboxes[i].config(fg="red")

    def unbind_devices():
        """Unbinds the selected devices."""
        global bound_devices
        checked_items = get_checked_items()
        
        # Unbind the selected device
        selected_devices = [devices[i] for i in checked_items]
        for device in selected_devices:
            device_name = device.split()[0]
            print(f"Unbinding {device_name}...")
            subprocess.run([
                "sudo",
                "usbip",
                "unbind",
                "-b",
                device_name,
            ], check=True)
            bound_devices.remove(device_name)
            
        # Update the checkboxes
        for i, device in enumerate(devices):
            device_name = device.split()[0]
            if device_name in bound_devices:
                device_checkboxes[i].config(fg="green")
            else:
                device_checkboxes[i].config(fg="red")
                
    # Set up the GUI
    root = tk.Tk()
    root.title("USB/IP Bindings")
    root.geometry("400x400")

    # Create the device list
    devices_label = tk.Label(root, text="Select Devices to Bind/Unbind:")
    devices_label.pack()

    device_frame = tk.Frame(root)
    device_frame.pack()

    devices = get_devices()
    device_checkboxes = []
    for device in devices:
        device_name = device.split()[0]
        checkbox = tk.Checkbutton(device_frame, text=device, onvalue=1, offvalue=0)
        if device_name in bound_devices:
            checkbox.config(fg="green")
            checkbox.select()
        else:
            checkbox.config(fg="red")
        checkbox.pack(anchor="w")
        device_checkboxes.append(checkbox)

    # Create the bind/unbind buttons
    button_frame = tk.Frame(root)
    button_frame.pack()

    bind_button = tk.Button(button_frame, text="Bind Selected Devices", command=bind_devices)
    bind_button.pack(side="left", padx=10)

    unbind_button = tk.Button(button_frame, text="Unbind Selected Devices", command=unbind_devices)
    unbind_button.pack(side="left", padx=10)

    # Run the GUI
    root.mainloop()
