import folium
import pandas as pd

# Step 1: Load the route information from CSV
df = pd.read_csv('route_output.csv')

# Step 2: Manually define locations (latitude, longitude) for each place
locations = {
    "GEU": (30.267894, 77.996734),
    "railway station": (30.3214, 78.0435),
    "clock tower": (30.3256, 78.0437),
    "FRI": (30.2715, 77.9948),
    "rispana pul": (30.2950, 78.0120),
    "ISBT": (30.3065, 78.0550)
}

# Step 3: Create a map centered at GEU initially
m = folium.Map(location=locations['GEU'], zoom_start=13)

# Step 4: Add markers for all the places involved in the route
for place in set(df['SOURCE']).union(set(df['DESTINATION'])):
    if place in locations:
        lat, lon = locations[place]
        folium.Marker(location=(lat, lon), popup=place, icon=folium.Icon(color='blue')).add_to(m)

# Step 5: Draw the path (lines) for the shortest route
for idx, row in df.iterrows():
    src = row['SOURCE']
    dest = row['DESTINATION']
    
    if src in locations and dest in locations:
        src_latlon = locations[src]
        dest_latlon = locations[dest]
        
        folium.PolyLine(locations=[src_latlon, dest_latlon], color="red", weight=5, opacity=0.8).add_to(m)

# Step 6: Save the map to an HTML file
m.save('route_map.html')

print("Map has been generated and saved as 'route_map.html'. Open it in your browser!")
