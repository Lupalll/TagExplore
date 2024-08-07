<?
require_once "../src/head/header.php"
?>

<?php


$servername = "localhost";
$username = "root";
$password = "";
$database = "TagExplore"; // Change this to your MySQL database name

// Create connection
$conn = mysqli_connect($servername, $username, $password, $database);

// Check connection
if (!$conn) {
  die("Connection failed: " . mysqli_connect_error());
  echo"connection failed". mysqli_connect_error();
  //console.log("connection failed". mysqli_connect_error());
}
// echo "Connected successfully <br/>";


// Annahme: Die UID wird als POST-Parameter mit dem Namen "uid" übertragen
// $description = $_POST['description'];
// $mode = $_POST['complete']; // Add this line to retrieve the mode
$uid = $_POST['uid'];
$mode = $_POST['mode'];
$description = $_POST['description'];

if ($mode == "delete") {
  // SQL-Befehl, um die UID in die Tabelle einzufügen, falls sie nicht existiert
  $sql = "DELETE FROM Tags WHERE id=(SELECT id FROM Tags WHERE description='$description' LIMIT 1);";
} else if ($mode == "post") {
  // delete or checkout item based in uid
  $sql = "DELETE FROM Tags WHERE id='$uid';";
}

// SQL-Befehl ausführen
if ($conn->query($sql) === TRUE) {
    echo "Element erfplgreich gelsöscht";
} else {
    echo "Fehler beim Löschen des elements: " . $conn->error;
}

// Verbindung schließen
$conn->close();
?>
?>
<?
require_once "../src/foot/footer.php"
?>
