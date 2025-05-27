import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15

Item {
    id: root  // ← nadaj ID żeby móc go odnaleźć z C++

    width: 800
    height: 600

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Map {
        id: map
        anchors.fill: parent
        visible: true
        plugin: mapPlugin
        center: QtPositioning.coordinate(51.106098, 17.060781)
        zoomLevel: 20

        MapQuickItem {
            id: boatItem
            coordinate: QtPositioning.coordinate(51.106098, 17.060781)
            anchorPoint.x: 12
            anchorPoint.y: 12
            sourceItem: Rectangle {
                width: 24
                height: 24
                color: "red"
                radius: 12
            }
        }

        MapPolyline {
            id: track
            line.width: 3
            line.color: "blue"
            path: []
        }
    }

    // Przeniesiona funkcja tutaj, do "root", żeby mogła być wołana z C++
function updateBoatPosition(lat, lon) {
    let coord = QtPositioning.coordinate(lat, lon)
    boatItem.coordinate = coord
    track.path.push(coord)
    // map.center = coord   <-- USUNIĘTE, żeby mapa się nie przesuwała
}

}
