import pandas as pd
import folium

# Load places and their coordinates
places_df = pd.read_csv('places.csv')

# Load all paths edges (pairs of lat/lon) from your C program output
all_paths_df = pd.read_csv('all_paths.csv')

# Load shortest path coordinates
shortest_path_df = pd.read_csv('shortest_path.csv')

# Initialize map centered roughly at average coordinates
center_lat = places_df['lat'].mean()
center_lon = places_df['lon'].mean()
m = folium.Map(location=[center_lat, center_lon], zoom_start=14)

# Add all edges (paths) with thin gray lines
for path_id in all_paths_df['path_id'].unique():
    segment = all_paths_df[all_paths_df['path_id'] == path_id]
    coords = list(zip(segment['lat'], segment['lon']))
    folium.PolyLine(
        coords,
        color='gray',
        weight=2,
        opacity=0.5
    ).add_to(m)

# Add shortest path polyline with distinct color and weight
shortest_coords = list(zip(shortest_path_df['lat'], shortest_path_df['lon']))
folium.PolyLine(
    shortest_coords,
    color='red',
    weight=5,
    opacity=0.8,
    tooltip='Shortest Path'
).add_to(m)

# Add markers for all places with popups and tooltips
for idx, row in places_df.iterrows():
    folium.Marker(
        location=[row['lat'], row['lon']],
        popup=folium.Popup(f"<b>{row['name']}</b>", max_width=200),
        tooltip=row['name'],
        icon=folium.Icon(color='blue', icon='info-sign')
    ).add_to(m)

# Save the map to HTML file
m.save('route_output.html')
print("Map saved as route_output.html")
