<?php
/**
 * La Classe che eredita le chiamate comuni per tutti i controlli
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

App::uses('Controller', 'Controller');

class AppController extends Controller {
    //Componenti che utilizziamo
    public $components = array(
        'RequestHandler',
        'RestAuth',
        'Session',
        'Auth' => array(
            'loginAction' => array('controller' => 'DocApi', 'action' => 'index')
        )
    );
    
    public $theme = 'HorusTheme';               //Tema corrente
    protected $is_demo = false;                 //Modalita Demo o meno
    protected $table_translate = NULL;
    
    //Modelli utilizzati
    public $uses = array(
        'ModHorusConfiguration',
    );
    
    public function beforeFilter() {
        parent::beforeFilter();
        
        //Imposta la lingua di chi sta facendo la richiesta
        $this->checkBrowserLanguage();
        
        //Importa le librerie di default
        App::uses('Sanitize', 'Utility');
                
        //Gestisce la Modalita demo
        $demo = $this->ModHorusConfiguration->findByCode('HORUS_MODE');
        $is_demo = ($demo['ModHorusConfiguration']['value'] == "demo");
        if($is_demo) {
            Configure::write('debug', 2);  //Logging demo
        } else {
            Configure::write('debug', 0);  //Logging produzione
        }
        
        //Debug Della Richiesta
	CakeLog::debug(print_r($_SERVER[ 'REQUEST_URI' ], true));
        CakeLog::debug("\n{$this->action} --- {$this->params['controller']}");
        CakeLog::debug($_SERVER['REQUEST_URI']."\n");
        
        //In caso di errore rimanda al gestoste di eccezione
        set_error_handler(array($this, 'exceptionErrorHandler'));
        
        //Fa il controllo di validazione per tutti ad eccezione della connesione di test
        if ($this->isREST() && $this->action != "connectionTest") {
            $this->theme = "empty";
            if (!(isset($this->request->query['time'])) || !(isset($this->request->query['hash']))) {
                $this->responseWithCode('Unauthorized', 'Authentication credentials missing.');
            } else {
                if ($this->action != 'badRequest') {
                    if (!$this->RestAuth->validateHash($this->request->query['time'], $this->request->query['hash'])) {
                        $this->responseWithCode('Unauthorized', 'Invalid authentication credentials.');
                        CakeLog::info("Non é autenticato"); 
                    } else {
                        CakeLog::info("autenticato");
                        $this->Auth->allow();
		    }
                }
            }
	} else if(!$this->isREST() && $this->params['controller'] != 'DocApi'){
           $this->redirect('/'); 
        }
        
        // Add a pattern value detector.
        $this->request->addDetector('android', array('env' => 'HTTP_USER_AGENT', 'pattern' => '/Android/i'));
    }
    
    /**
     * Controlla se e' un applicazione RESTful o meno
     * @return boolean restituisce true se e' una richiesta Restfull oppure false altrimenti
     */
    protected function isREST() {
        $ext = isset($this->RequestHandler->request->params['ext']) ? $this->RequestHandler->request->params['ext'] : '';
        if ($ext == 'json' || $ext == 'xml' || $ext == "dat" || $ext == "jpeg")
            return true;
        else
            return false;
        
    }
    
    /**
     * Gestisce la risposta per una richiesta di tipo RestFull della applicazione
     * @param string $description descrizione della risposta da mandare
     * @param string $message messaggio da mandare alla richiesta
     * @throws Exception lancia l'eccezione se la richiesta presenta un qualche errore di validazione
     * @throws NotFoundException lancia l'eccezione se la risorsa che si richiede non e'presente
     * @throws MethodNotAllowedException lancia l'eccezione se la richiesta non e' permessa
     * @throws InternalErrorException lancia l'eccezione se la richiesta genera un errore interno al server
     */
    protected function responseWithCode($description, $message = '') {

        switch ($description) {
            /*
             * La richiesta ha successo. Le informazioni restituite con la risposta e'
             * dipendende al metodo usato nella richiesta
             */
            case 'Ok':
                $this->response->statusCode(200);
                break;
            /*
             * The request has been fulfilled and resulted in a new resource being created. 
             * The newly created resource can be referenced by the URI(s) returned 
             * in the entity of the response, with the most specific URI for the 
             * resource given by a Location header field. The response SHOULD include 
             * an entity containing a list of resource characteristics and location(s) 
             * from which the user or user agent can choose the one most appropriate. 
             * The entity format is specified by the media type given in the Content-Type 
             * header field. The origin server MUST create the resource before returning 
             * the 201 status code. If the action cannot be carried out immediately, 
             * the server SHOULD respond with 202 (Accepted) response instead
             */
            case 'Created':
                $this->response->statusCode(201);
                break;
            /*
             * The request has been accepted for processing, but the processing has 
             * not been completed. The request might or might not eventually be 
             * acted upon, as it might be disallowed when processing actually takes 
             * place. There is no facility for re-sending a status code from an 
             * asynchronous operation such as this.
             */
            case 'Accepted':
                $this->response->statusCode(202);
                break;
            /*
             * This status code is usually sent out in response to a PUT, POST, or 
             * DELETE request, when the server declines to send back any status 
             * message or representation. The server may also send 204 in conjunction 
             * with a GET request: the resource requested exists, but has an empty 
             * representation. Compare 304 (“Not Modified”).
             */
            case 'NoContent':
                $this->response->statusCode(204);
                break;
            /*
             * Sent when the client triggers some action that causes the URI of a 
             * resource to change. Also sent if a client requests the old URI.
             */
            case 'MovedPermanently':
                $this->response->statusCode(301);
                throw new Exception($message, 301);
                break;
            /*
             * This status code is similar to 204 (“No Content”) in that the response 
             * body must be empty. But 204 is used when there is no body data to send, 
             * and 304 is used when there is data but the client already has it. 
             * There’s no point in sending it again.
             */
            case 'NotModified':
                $this->response->statusCode(304);
                throw new Exception($message, 304);
                break;
            /*
             * This is the generic client-side error status, used when no other 
             * 4xx error code is appropriate. It’s commonly used when the client 
             * submits a representation along with a PUT or POST request, 
             * and the representation is in the right format, but it doesn’t make any sense.
             */
            case 'BadRequest':
                $this->response->statusCode(400);
                throw new Exception($message, 400);
                break;
            /*
             * The client tried to operate on a protected resource without providing 
             * the proper authentication credentials. It may have provided the wrong 
             * credentials, or none at all. The credentials may be a username and 
             * password, an API key, or an authentication token—whatever the service 
             * in question is expecting. It’s common for a client to make a request 
             * for a URI and accept a 401 just so it knows what kind of credentials 
             * to send and in what format. [...]
             */
            case 'Unauthorized':
                $this->response->statusCode(401);
                throw new Exception($message, 401);
                break;
            /*
             * The server understood the request, but is refusing to fulfill it. 
             */
            case 'Forbidden':
                $this->response->statusCode(403);
                throw new Exception($message, 403);
                break;
            /*
             * Sent when the client requests a URI that doesn’t map to any resource. 
             * 404 is used when the server has no clue what the client is asking for. 410 is used when the server knows there used to be a resource there, but there isn’t anymore.
             */
            case 'NotFound':
                $this->response->statusCode(404);
                throw new NotFoundException($message);
                break;
            /*
             * The client tried to use an HTTP method that this resource doesn’t support. 
             * For instance, a read-only resource may support only GET and HEAD. 
             * Another resource may allow GET and POST, but not PUT or DELETE.
             */
            case 'MethodNotAllowed':
                $this->response->statusCode(405);
                throw new MethodNotAllowedException($message);
                break;
            /*
             * The request could not be completed due to a conflict with the current 
             * state of the resource. 
             */
            case 'Conflict':
                $this->response->statusCode(409);
                throw new Exception($message, 409);
                break;
            /*
             * Sent when the client requests a URI that doesn’t map to any resource. 
             * 410 is used when the server knows there used to be a resource there, 
             * but there isn’t anymore.
             */
            case 'Gone':
                $this->response->statusCode(410);
                throw new Exception($message, 410);
                break;
            /*
             * This is the generic server error response.
             */
            case 'InternalServerError':
                $this->response->statusCode(500);
                throw new InternalErrorException($message, 500);
                break;
        }
    }
    
    /**
     * Risponde come errore di richiesta
     */
    public function badRequest() {
        $this->responseWithCode('BadRequest', 'Bad request, please retry.');
    }

    /**
     * Gestisce le procedure dopo l'esecuzione del controller
     */
    public function afterFilter() {
        parent::afterFilter();

        //Gestisce la password temporanea durante la creazione di un nuovo utente
        if($this->action != "putUser" || $this->params['controller']!= "Authmanager") {
            $this->ModHorusConfiguration->deleteAll(array(
                "code" => "TEMP_PASSWORD"
            ));
        }
    }
    
    /**
     * Legge dalla richiesta la lingua e la imposta se è disponibile.
     */
    protected function checkBrowserLanguage() {
        App::import('Vendor','Gettext/MO'); 
        
        if(isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])){
            $browserLanguage = ""; 
            $x = explode(",", $_SERVER['HTTP_ACCEPT_LANGUAGE']);
            
            foreach ($x as $val) {
                if($val != 'null' && $val != 'NULL') {
                    $browserLanguage = $val;
                    break;
                }
            }
            
            $browserLanguage = substr($browserLanguage, 0, 2);
             
            $lang = NULL;
            //available languages
            switch ($browserLanguage){
                case "en":
                    $this->Session->write('Config.language', 'eng');
                    break;
                case "it":
                    $this->Session->write('Config.language', 'ita');
                    $lang = "ita";
                    break;
            }
            
            if($lang != NULL) {
                $file = new File_Gettext_MO();
                $file->load(APP."Locale".DS.$lang.DS."LC_MESSAGES".DS."default.mo");
                $this->table_translate = array_flip($file->strings);
            }
        }
    }
    
    /**
     * Handler per l'invio degli errori
     * @param int $errno numero di errore
     * @param string $errstr stringa con l'informazione del errore
     * @param string $errfile nome del file che ha generato l'errore
     * @param int $errline linea di dove si e' verificato l'errore
     * @throws ErrorException rilancia l'eccezione in base le informazioni ottenute come parametro
     */
    protected function exceptionErrorHandler($errno, $errstr, $errfile, $errline ) {
       throw new ErrorException($errstr, $errno, 0, $errfile, $errline);
    }
}
