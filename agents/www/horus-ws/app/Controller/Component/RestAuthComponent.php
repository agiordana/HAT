<?php

/**
 * Description of SecurityComponent
 *
 * @author Davide Monfrecola<davide.monfrecola@gmail.com>
 */
class RestAuthComponent extends Component {
    
    private $api_key;
    private $validity;
    const APPLICATION_ID_LENGTH = 25;
    
    private static $seed = 0;
    private static  $a = 3;
    private static  $c = 9;
    private static  $m = 16;
    
    /**
     * Calcola hash hmac da utilizzare per l'autenticazione con il web service 
     * 
     * @return array Array associativo che contiene il timestamp e l'hash da 
     *               utilizzare per la chiamata al web service
     */
    public function getHashHmac($url) {
        $time = time();    // validità di un minuto del messaggio
        
        //Chiave Da Applicare
        $api_key = $this->controller->ModHorusConfiguration->findByCode('APPLICATION_ID');
        $this->api_key = $api_key["ModHorusConfiguration"]["value"];
        
        $my_sign = hash_hmac("sha1", $url.$time, $this->api_key, true);
        $my_sign = base64_encode($my_sign);
        
        return array(
            'time' => $time,
            'hash' => $my_sign
        ); 
    }
    
    /**
     * Il metodo viene richiamato dopo l'esecuzione del metodo beforeFilter()
     * ma prima che il controller esegue l'azione richiesta
     * 
     * @param type $controller puntatore al controller
     */
    public function initialize(Controller $controller) {
        $this->agent_version = Configure::read('Horus.agent_version');
        $this->controller = $controller;
    }
    
    /**
     * Controlla che l'hash passato dal client sia corretto
     * 
     * @param int Timestamp chiamata
     * @param string Hash inviato dal client calcolato tramite algortimo hash hmac
     */
    public function validateHash($time, $hash) {
        //Tempo di Validazione
        $this->validity = Configure::read('Horus.api_validity');
        
        //Chiave Da Applicare
        $api_key = $this->controller->ModHorusConfiguration->findByCode('APPLICATION_ID');
        $this->api_key = $api_key["ModHorusConfiguration"]["value"];
        
        $expire_before = strtotime('-'.$this->validity.' seconds');
        $expire_after = strtotime('+'.$this->validity.' seconds');
        
        $url = Router::url(null, true );
        CakeLog::debug("\n\ninformazione:".$url.$time);
        
	$my_sign = hash_hmac("sha1", $url.$time, $this->api_key, true);
        $my_sign = base64_encode($my_sign);

        CakeLog::debug("\n\nhash:".$my_sign);
        
        // controlla che hash siano uguali e che il tempo della chiaamta sia
        // ancora valido (range di +- valdita espressa in secondi)
        if($my_sign == $hash && ($time >= $expire_before) && ($time <= $expire_after)) {
            return true;
        }

	if($my_sign == $hash){
	   return true;
       } else {
            CakeLog::info("[SecurityComponent::validateHash] Invalid request from client");
            CakeLog::info("$time >=".(((int)$now) -(int)$this->validity)." $time <=".(((int)$now) +(int)$this->validity));
            return false;
        }
    }
    
    /**
     * Generatore di numero casuale LCG
     */
    public static function rand_n() {
        CakeLog::error("A:".self::$a."\nSeed:".self::$seed);
        CakeLog::error("C:".self::$c."\nM:".self::$m."===============\n\n");
        
        self::$seed = 1 + ((self::$a * self::$seed + self::$c) % self::$m);
        return self::$seed;
    }

    /**
     * Imposta il seme
     * @param type $seed
     */
    public static function srand_n($seed) {
        self::$seed = $seed;
    }
}

?>
