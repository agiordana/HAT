<?php
/**
 * Controlli preliminari delle webservices
 *
 * Alessandro Gallina e Enrico Fabio
 * Copyright (c) Penta Dynamic Solutions S.r.l
 *
 *
 * @copyright     Penta Dynamic Solutions S.r.l
 * @link          http://pentads.it
 * @package       app.Controller
 * @since         Configurator 1.0
 */
class HorusController extends AppController {
    
    /**
     * Abilita la chiamata di connessione di test senza bisogno di autenticazione
     */
    public function beforeFilter() {
        $this->Auth->allow('connectionTest', 'check');
        parent::beforeFilter();
    }

    /**
     * Effettua il test della chiave
     * REST url: /Horus/check.json (GET)
     */
    public function check() {
        if ($this->isREST()) {
            $this->set(array(
                'response' => array("message" => "OK"),
                '_serialize' => array('response')
            ));
        } else {
            $this->responseWithCode('Forbidden');
        }
    }

    /**
     * Effettua il test della chiave
     * REST url: /aya.json (GET)
     */
    public function connectionTest() {
        if ($this->isREST()) {
            $this->set(array(
                'response' => array("message" => "IAA"),
                '_serialize' => array('response')
            ));
        } else {
            echo json_encode(array(
                'response' => array("message" => "IAA")
            ));
            exit();
        }
    }
    
    /**
     * Genera la nuova chiave di cifratura
     */
    public function genKey() {
        $config = $this->ModHorusConfiguration->findByCode('APPLICATION_ID');
        $api_key = $config["ModHorusConfiguration"]["value"];
        
        //Generazione del numero casuale
        CakeLog::debug("\n===============\nTimestamp:".$this->request->query['time']);
        
        RestAuthComponent::srand_n((int)substr($this->request->query['time'], -3));
        $n = RestAuthComponent::rand_n();
        
        CakeLog::debug("numero:".$n);
        
        $my_sign = $api_key;
        for($i=0; $i < $n; $i++) {
            $key = hexdec(substr($my_sign, -3)) % $n;
            $my_sign = hash_hmac("sha1", $my_sign, $key, true);
            $my_sign = substr(bin2hex($my_sign), 0, RestAuthComponent::APPLICATION_ID_LENGTH);
        }
        
        //Salvo la nuova chiave generata
        $config["ModHorusConfiguration"]["value"] = $my_sign;
        $this->ModHorusConfiguration->save($config);
        
        if ($this->isREST()) {
            $this->set(array(
                'response' => array("message" => "OK"),
                '_serialize' => array('response')
            ));
        } else {
            echo json_encode(array(
                'response' => array("message" => "OK")
            ));
            exit();
        }
    }
}

?>
