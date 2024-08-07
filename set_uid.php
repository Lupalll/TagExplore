<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "TagExplore";

// Verbindung herstellen
$conn = new mysqli($servername, $username, $password, $dbname);

// Überprüfen der Verbindung
if ($conn->connect_error) {
    die("Verbindung fehlgeschlagen: " . $conn->connect_error);
}
echo "Erfolgreich verbunden";

// Annahme: Die UID wird als POST-Parameter mit dem Namen "uid" und die Beschreibung als "description" übertragen
$uid = $_POST['uid'];
$description = $_POST['description'];
$mode = $_POST['mode']; // Add this line to retrieve the mode

if ($mode == "insert") {
  // SQL-Befehl, um die UID in die Tabelle einzufügen, falls sie nicht existiert
  $sql = "INSERT INTO Tags (id, description) SELECT '$uid', '$description' FROM dual WHERE NOT EXISTS (SELECT * FROM Tags WHERE id = '$uid')";
} else if ($mode == "count") {
  // In count mode, insert only the UID without considering the description
  $sql = "INSERT INTO Tags (id) SELECT '$uid' FROM dual WHERE NOT EXISTS (SELECT * FROM Tags WHERE id = '$uid')";
}

// SQL-Befehl ausführen
if ($conn->query($sql) === TRUE) {
    echo "Neue UID erfolgreich eingefügt";
} else {
    echo "Fehler beim Einfügen der UID: " . $conn->error;
}

// Verbindung schließen
$conn->close();
?>
<!DOCTYPE html>
<html>
<head>
<title>Title of the document</title>
</head>

<body>
The content of the document......
</body>

</html>
