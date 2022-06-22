var dashboardMap = L.map("mapid").setView([39.72, -7.99], 7);

L.tileLayer('https://api.mapbox.com/styles/v1/{id}/tiles/{z}/{x}/{y}?access_token={accessToken}', {
    attribution: 'Map data &copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors, Imagery © <a href="https://www.mapbox.com/">Mapbox</a>',
    maxZoom: 18,
    id: 'mapbox/streets-v11',
    tileSize: 512,
    zoomOffset: -1,
    accessToken: 'pk.eyJ1IjoicmFicmFudGVzIiwiYSI6ImNrdGl1NnpsdDB2NDYycW16aWQ0a3FvcnkifQ.1GPQKAfxzk1K9D9voL-fdg'
}).addTo(dashboardMap);
