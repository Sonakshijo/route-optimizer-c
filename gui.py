import tkinter as tk
from tkinter import ttk, messagebox
import subprocess
import webbrowser
import os
import csv
import sys

def load_locations_from_csv(csv_file="graph.csv"):
    locations = set()
    if not os.path.exists(csv_file):
        return []
    with open(csv_file, newline='') as f:
        reader = csv.reader(f)
        next(reader)  # skip header
        for row in reader:
            if len(row) >= 2:
                locations.add(row[0].strip())
                locations.add(row[1].strip())
    return sorted(list(locations))

locations = load_locations_from_csv()
categories = [
    "hospital", "bus stop", "police station", "market", "bridge", "road",
    "chauk", "colony", "locality", "landmark", "university", "park", "railway station"
]
def run_route_optimizer():
    source = source_combo.get()
    dest = dest_combo.get()
    cat = category_combo.get()

    if not source or not dest or not cat:
        messagebox.showerror("Input Error", "Please select source, destination, and category.")
        return

    try:
        user_input = f"{source}\n{dest}\n{cat}\n"
        result = subprocess.run(
            ['route_optimizer.exe'],
            input=user_input,
            text=True,
            capture_output=True,
            check=True
        )
    except subprocess.CalledProcessError as e:
        output_text.delete(1.0, tk.END)
        output_text.insert(tk.END, "Error running optimizer:\n")
        output_text.insert(tk.END, f"Return code: {e.returncode}\n")
        output_text.insert(tk.END, f"Stdout:\n{e.stdout}\n")
        output_text.insert(tk.END, f"Stderr:\n{e.stderr}\n")
        messagebox.showerror("Route Optimizer Error", "Error running optimizer. Check output box for details.")
        return

    output_text.delete(1.0, tk.END)
    output_text.insert(tk.END, result.stdout)

    # Run the output.py script to generate the map
    output_py_path = os.path.abspath("output.py")
    python_exe = sys.executable

    try:
        map_result = subprocess.run(
            [python_exe, output_py_path],
            capture_output=True,
            text=True,
            check=True
        )
    except subprocess.CalledProcessError as e:
        messagebox.showerror("Map Generation Error", f"Error generating map:\n{e.stderr}")
        return

    html_path = os.path.abspath("route_output.html")
    if os.path.exists(html_path):
        webbrowser.open_new_tab(f"file:///{html_path.replace(os.sep, '/')}")
    else:
        messagebox.showinfo("Map File", "Map HTML not found.")

def create_tooltip(widget, text):
    tooltip = tk.Toplevel(widget)
    tooltip.withdraw()
    tooltip.overrideredirect(True)
    label = tk.Label(tooltip, text=text, background="yellow", relief="solid", borderwidth=1, font=("tahoma", "8", "normal"))
    label.pack()

    def enter(event):
        x = event.x_root + 20
        y = event.y_root + 10
        tooltip.geometry(f"+{x}+{y}")
        tooltip.deiconify()

    def leave(event):
        tooltip.withdraw()

    widget.bind("<Enter>", enter)
    widget.bind("<Leave>", leave)

root = tk.Tk()
root.title("Route Optimization System")

tk.Label(root, text="Source:").grid(row=0, column=0, padx=10, pady=10, sticky='w')
source_combo = ttk.Combobox(root, values=locations, state="readonly")
source_combo.grid(row=0, column=1)
create_tooltip(source_combo, "Select the starting location")

tk.Label(root, text="Destination:").grid(row=1, column=0, padx=10, pady=10, sticky='w')
dest_combo = ttk.Combobox(root, values=locations, state="readonly")
dest_combo.grid(row=1, column=1)
create_tooltip(dest_combo, "Select the destination location")

tk.Label(root, text="Category:").grid(row=2, column=0, padx=10, pady=10, sticky='w')
category_combo = ttk.Combobox(root, values=categories, state="readonly")
category_combo.grid(row=2, column=1)
create_tooltip(category_combo, "Select category to find nearest location")

tk.Button(root, text="Find Route", command=run_route_optimizer).grid(row=3, column=0, columnspan=2, pady=20)

output_text = tk.Text(root, height=15, width=70)
output_text.grid(row=4, column=0, columnspan=2, padx=10, pady=10)

root.mainloop()
