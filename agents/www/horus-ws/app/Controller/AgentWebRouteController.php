<?php
/**
 * Inoltro ai messaggi al agente wsserver
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
class AgentWebRouteController extends AppController {
    const AGENT_PORT = 14990; 
    /**
     * Inoltro dei messaggi verso l'agente
     */
    public function forward() {
        App::uses('HttpSocket', 'Network/Http');
        
        $HttpSocket = new HttpSocket(array(
            'ssl_verify_peer' => false,
            'ssl_allow_self_signed' => true
        ));
        
        $path = $this->request->here(false);
/*        $url = "http://".$_SERVER['HTTP_HOST'].":".self::AGENT_PORT.'/horus-ws'.$path;  */
        $url = "http://localhost:".self::AGENT_PORT.'/horus-ws'.$path; 
        
        $resp = null;
        switch ($this->request->method()) {
            case 'POST':
                $data = $this->request->input();
                $resp = $HttpSocket->post($url, $data);
                break;
            case 'GET':
                $resp = $HttpSocket->get($url);
                break;
            case 'DELETE':
                $resp = $HttpSocket->delete($url);
                break;
            case 'PUT':
                $data = $this->request->input();
                $resp = $HttpSocket->put($url, $data);
                break;
        }
        if($resp!= null && $resp->isOk()) {
            $agent_response = (array) json_decode($resp->body());
            $agent_response['_serialize'] = array('response');
            $this->set($agent_response);
        } else {
            $this->badRequest(); 
        }
    }
}
?>

