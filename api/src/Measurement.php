<?PHP
include_once('Database.php');
include_once('Sensor.php');

class Measurement
{
  public int $id;
  public int $vehicle;
  public int $time;
  public array $values;
  public array $sensors;

  public function __construct($id, $vehicle, $time, $values, $sensors)
  {
    $this->id = $id;
    $this->vehicle = $vehicle;
    $this->time = $time;
    $this->values = $values;
    $this->sensors = $sensors;
  }

  public static function exists($id) : bool
  {
    $db = new Database();
    $query = "SELECT id FROM samples WHERE id = $id;";
    $result = $db->query($query);
    $num = $result->num_rows;
    $result->free();
    return $num > 0;
  }

  public static function number($vehicle_id) : int
  {
    $db = new Database();
    $query = "SELECT COUNT(id) AS 'count' FROM samples WHERE vehicle_id = " .
      "$vehicle_id;";
    $result = $db->query($query);
    $row = $result->fetch_assoc();
    $num = $row['count'];
    $result->free();
    return $num;
  }

  public static function deleteMeasurement($id) : void
  {
    $db = new Database();
    $query = "DELETE FROM samples WHERE id = $id;";
    $db->query($query);
    return;
  }

  public static function deleteAllMeasurements($id) : void
  {
    $db = new Database();
    $query = "DELETE FROM samples WHERE vehicle_id = $id;";
    $db->query($query);
    return;
  }

  public static function createMeasurement($vehicle_id, $sensors, $values): int
  {
    if(count($sensors) != count($values))
    {
      throw new \InvalidArgumentException('Invalid pairs of 
        sensors and values');
      die();
    }

    $vehicle_sensors = Sensor::getSensors($vehicle_id);
    foreach($sensors as $input)
    {
      $found = false;
      foreach($vehicle_sensors as $sensor)
      {
        if($sensor->id == $input)
        {
          $found = true;
          break;
        }
      }

      if(!$found)
      {
        throw new \InvalidArgumentException('Some of the given sensors do not
          belong to this vehicle');
        die();
      }
    }

    $db = new Database();
    $query = "INSERT INTO samples (vehicle_id) VALUE ($vehicle_id);";
    $db->query($query);

    $result = $db->query("SELECT @@IDentity as 'id'");
    $row = $result->fetch_assoc();
    $sample_id = $row['id'];
    $result->free();

    for($i=0; $i<count($sensors); $i++)
    {
      $query = 'INSERT INTO measurements (sensor_id, sample_id, value) VALUE 
        (' . $sensors[$i] . ',' . $sample_id . ',\'' . $values[$i] . '\');';
      $db->query($query);
    }

    return $sample_id;
  }

  public static function getMeasurement($id) : Measurement
  {
    $db = new Database();
    $query = "SELECT timestamp, vehicle_id FROM samples WHERE id = $id;";
    $result = $db->query($query);
    $row = $result->fetch_assoc();

    $vehicle = $row['vehicle_id'];
    $time = DateTime::createFromFormat('Y-m-d G:i:s.v', $row['timestamp'],
      new DateTimeZone('Europe/Lisbon'));
    $time = $time->format('U') * 1000 + $time->format('v');

    $query = "SELECT sensor_id, value FROM measurements WHERE sample_id = $id;";
    $result = $db->query($query);

    $sensors = [];
    $values = [];
    foreach ($result as $row)
    {
      $sensors[] = $row['sensor_id'];
      $values[] = $row['value'];
    }
    $result->free();

    $measurement = new Measurement($id, $vehicle, $time, $values, $sensors);
    return $measurement;
  }

  public static function getMeasurementsLimitOffset($vehicleId, $limit, 
    $offset = 0) : array
  {
    $db = new Database();
    $query = "SELECT id FROM samples WHERE vehicle_id = $vehicleId ORDER BY" .
     " id DESC LIMIT $limit OFFSET $offset;";
    $result = $db->query($query);

    $measurements = [];
    foreach($result as $row)
    {
      $measurements[] = Measurement::getMeasurement($row['id']);
    }
    $result->free();
    return $measurements;
  }

  public static function getMeasurements($vehicleId) : array
  {
    $db = new Database();
    $query = "SELECT id FROM samples WHERE vehicle_id = $vehicleId ORDER BY " .
      "id DESC;";
    $result = $db->query($query);

    $measurements = [];
    foreach($result as $row)
    {
      $measurements[] = Measurement::getMeasurement($row['id']);
    }
    $result->free();
    return $measurements;
  }
}
?>
