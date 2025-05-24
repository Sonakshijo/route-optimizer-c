import openrouteservice

import folium
import openrouteservice
from openrouteservice import convert

client = openrouteservice.Client(key='5b3ce3597851110001cf62482a5185a1fa974c828851b51f0a344e8e')

coords = [(78.0790, 30.2849), (78.0322, 30.3076)]  # (lon, lat) format

# Get route
route = client.directions(coords, profile='foot-walking', format='geojson')

# Create map
m = folium.Map(location=[30.295, 78.055], zoom_start=13)

# Add route to map
folium.GeoJson(route, name="route").add_to(m)

# Add markers
folium.Marker(location=coords[0][::-1], popup="Start").add_to(m)
folium.Marker(location=coords[1][::-1], popup="End").add_to(m)

m.save("smart_route.html")
