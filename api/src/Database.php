<?PHP
class Database
{
  private $connection;

  public function __construct(){
    if(!isset($_ENV['DATABASE_USER']) || !isset($_ENV['DATABASE_PASSWORD']) || 
      !isset($_ENV['DATABASE_NAME']))
    {
      throw new \RuntimeException('The environment variables were not 
        initialized');
    }

    mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
    $this->connection = mysqli_connect('database:3306', $_ENV['DATABASE_USER'], 
      $_ENV['DATABASE_PASSWORD'], $_ENV['DATABASE_NAME']);
  }

  public function query(string $query)
  {
    return $this->connection->query($query);
  }
}
?>
