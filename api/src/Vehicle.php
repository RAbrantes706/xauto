<?PHP
include_once('Database.php');
include_once('User.php');

class Vehicle
{
  public int $id;
  public string $name;
  public string $color;
  public int $owner;

  public function __construct($id, $name, $color, $owner)
  {
    $this->id = $id;
    $this->name = $name;
    $this->color = $color;
    $this->owner = $owner;
  }

  public static function exists($id) : bool
  {
    $db = new Database();
    $query = "SELECT id FROM vehicles WHERE id = $id;";
    $result = $db->query($query);
    $num = $result->num_rows;
    $result->free();
    return $num > 0;
  }

  public static function deleteVehicle($id) : void
  {
    $db = new Database();
    $query = "DELETE FROM vehicles WHERE id = $id;";
    $db->query($query);
    return;
  }

  public static function updateVehicle($id, $name, $color) : void
  {
    $db = new Database();
    $query = "UPDATE vehicles SET name = '$name', color = '$color' WHERE " . 
      "id = $id;";
    $db->query($query);
    return;
  }

  public static function createVehicle($userId, $name, $color) : int
  {
    $db = new Database();
    $query = "INSERT INTO vehicles (name, color, user_id) VALUES " .
      "('$name', '$color', $userId);";
    $db->query($query);

    $result = $db->query("SELECT @@IDentity as 'id'");
    $row = $result->fetch_assoc();
    $id = $row['id'];
    $result->free();
    return $id;
  }

  public static function getVehicle($id) : Vehicle
  {
    $db = new Database();
    $query = "SELECT * FROM vehicles WHERE id = $id;";
    $result = $db->query($query);
    $row = $result->fetch_assoc();

    $vehicle = new Vehicle(intval($row['id']), $row['name'], 
      $row['color'], intval($row['user_id']));
    $result->free();
    return $vehicle;
  }

  public static function getUserVehicles($userId) : array
  {
    $db = new Database();
    $query = "SELECT id, name, color FROM vehicles WHERE user_id = $userId;";
    $result = $db->query($query);

    $vehicles = [];
    foreach($result as $row)
    {
      $vehicles[] = new Vehicle(intval($row['id']), $row['name'],
        $row['color'], $userId);
    }
    $result->free();
    return $vehicles;
  }

  public static function getAllVehicles() : array
  {
    $db = new Database();
    $result = $db->query("SELECT * FROM vehicles;");

    $vehicles = [];
    foreach($result as $row)
    {
      $vehicles[] = new Vehicle(intval($row['id']), $row['name'], 
        $row['color'], intval($row['user_id']));
    }
    $result->free();
    return $vehicles;
  }
}
?>
