<?PHP
include_once('Database.php');
include_once('Vehicle.php');

class Sensor
{
  public int $id;
  public string $name;
  public string $data_type;
  public int $vehicle;

  public function __construct($id, $name, $data_type, $vehicle)
  {
    $this->id = $id;
    $this->name = $name;
    $this->data_type = $data_type;
    $this->vehicle = $vehicle;
  }

  public static function exists($id) : bool
  {
    $db = new Database();
    $query = "SELECT id FROM sensors WHERE id = $id;";
    $result = $db->query($query);
    $num = $result->num_rows;
    $result->free();
    return $num > 0;
  }

  public static function deleteSensor($id) : void
  {
    $db = new Database();
    $query = "DELETE FROM sensors WHERE id = $id;";
    $db->query($query);
    return;
  }

  public static function updateSensor($id, $name, $data_type) : void
  {
    $db = new Database();
    $query = "UPDATE sensors SET name = '$name', type = '$data_type' " . 
      "WHERE id = $id;";
    $db->query($query);
    return;
  }

  public static function createSensor($vehicleId, $name, $data_type) : int
  {
    $db = new Database();
    $query = "INSERT INTO sensors (name, type, vehicle_id) VALUES ('$name', " .
      "'$data_type', $vehicleId);";
    $db->query($query);

    $result = $db->query('SELECT @@IDentity as \'id\'');
    $row = $result->fetch_assoc();
    $id = $row['id'];
    $result->free();
    return $id;
  }

  public static function getSensor($id) : Sensor
  {
    $db = new Database();
    $query = "SELECT * FROM sensors WHERE id = $id;";
    $result = $db->query($query);
    $row = $result->fetch_assoc();

    $sensor = new Sensor($id, $row['name'], $row['type'], 
      intval($row['vehicle_id']));
    $result->free();
    return $sensor;
  }

  public static function getSensors($vehicleId) : array
  {
    $db = new Database();
    $query = "SELECT id, name, type FROM sensors WHERE vehicle_id=$vehicleId;";
    $result = $db->query($query);

    $sensors = [];
    foreach($result as $row)
    {
      $sensors[] = new Sensor(intval($row['id']), $row['name'], $row['type'], 
        $vehicleId);
    }
    $result->free();
    return $sensors;
  }
}
?>
