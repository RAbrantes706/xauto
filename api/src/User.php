<?PHP
include_once 'Database.php';

class User
{
  public $id;
  public $name;
  public $password;
  public $email;

  public function __construct($id, $name, $password, $email)
  {
    $this->id       = $id;
    $this->name     = $name;
    $this->password = $password;
    $this->email    = $email;
  }

  public function verifyPassword($password) : bool
  {
    return password_verify($password, $this->password);
  }

  public static function exists($id) : bool
  {
    $db = new Database();
    $query = 'SELECT id FROM users WHERE id=' . $id . ';';
    $result = $db->query($query);
    $num = $result->num_rows;
    $result->free();
    return $num > 0;
  }

  public static function deleteUser($id) : void
  {
    $db = new Database();
    $query = 'DELETE FROM users WHERE id=' . $id . ';';
    $db->query($query);
    return;
  }

  public static function updateUser($id, $name, $password, $email) : void
  {
    $db = new Database();
    $pw = password_hash($password, PASSWORD_DEFAULT);
    $query = "UPDATE users SET name='$name', password=$pw, email=$email WHERE".
     " id=$id;";
    $db->query($query);
    return;
  }

  public static function createUser($name, $password, $email) : int
  {
    if(User::findUserByName($name) || User::findUserByEmail($email))
    {
      return -1;
    }

    $db = new Database();
    $pw = password_hash($password, PASSWORD_DEFAULT);
    $query = "INSERT INTO users (name, password, email) VALUE ('$name', '$pw'".
      ", '$email');";
    $db->query($query);

    $result = $db->query('SELECT @@IDentity as \'id\'');
    $row = $result->fetch_assoc();
    $id = $row['id'];
    $result->free();
    return $id;
  }

  public static function findUserByName($name) : ?User
  {
    return User::findUser('name', $name);
  }

  public static function findUserByEmail($email) : ?User
  {
    return User::findUser('email', $email);
  }

  private static function findUser($type, $data) : ?User
  {
    $db = new Database();
    $query = "SELECT * FROM users WHERE $type='$data';";
    $result = $db->query($query);

    if(!$result->num_rows)
    {
      $result->free();
      return null;
    }

    $row = $result->fetch_assoc();
    $user = new User(intval($row['id']), $row['name'], $row['password'], 
      $row['email']);
    $result->free();
    return $user;
  }

  public static function getUser($id) : User
  {
    $db = new Database();
    $query = "SELECT id, name, password, email FROM users WHERE id='$id';";
    $result = $db->query($query);
    $row = $result->fetch_assoc();

    $user = new User(intval($row['id']), $row['name'], $row['password'], 
      $row['email']);
    $result->free();
    return $user;
  }

  public static function getAllUsers() : array
  {
    $db = new Database();
    $result = $db->query('SELECT id, name, password, email FROM users;');

    $users = [];
    foreach($result as $row)
    {
      $users[] = new User(intval($row['id']), $row['name'], $row['password'], 
        $row['email']);
    }
    $result->free();
    return $users;
  }
}
?>
