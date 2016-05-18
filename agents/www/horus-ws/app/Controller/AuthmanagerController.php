<?php
/**
 * Gestione della autenticazione e parental control
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
class AuthmanagerController extends AppController {

    //Modelli in aggiunta a quelli ereditati da AppController
    public $uses = array('ModHorusUser', 'ModHorusUserHasExtAvailable','ModHorusExtAvailable');
      
    /**
     * Permette l'autenticazione classica dal app esterna
     * @param string $passwd password di autenticazione
     * @throws Exception lancia l'eccezione se la password non e' corretta oppure vuota
     */
    public function externalAuth($passwd = NULL){
       if ($this->isREST()) {
           try {
            $passwd = Sanitize::clean($passwd);
            if($passwd == NULL) {
                throw new Exception('empty password');
            } else {
                $user = $this->ModHorusUser->findByPassword($passwd/*AuthComponent::password($passwd)*/);
            }
            if(empty($user)) {
                throw new Exception('Wrong password');
            }
            
            //Utente non valido
            $this->set(array(
                'response' => array(
                     'message' => 'ok',
                     'user' => $user['ModHorusUser']['name']
                ),
                '_serialize' => array('response')
            ));
           } catch(Exception $e){
               CakeLog::error("[Authmanager::authenticate] {$e->getMessage()}");
               $this->responseWithCode('Forbidden', 'Wrong Password.');
           }
       }
    }

    /**
     * Ottiene la lista degli utenti
     * REST url: /Configuration/Users.json (GET)
     */
    public function getUserList() {
        try {
            $users = $this->ModHorusUser->find('all');
            
            $map = function($value) {
                $item = array(
                    'name' => $value['ModHorusUser']['name'],
                    'enable_status' => $value['ModHorusUser']['enable'] == 1
                );
                return $item;
            };
            $users = array_map($map, $users);

            if ($this->isREST()) {
                $this->set(array(
                    'response' => array(
                        'message' => 'ok',
                        'users' => $users
                    ),
                    '_serialize' => array('response')
                ));
            } else {
                echo json_encode(array(
                    'response' => array(
                        'message' => 'ok',
                        'users' => $users
                    ))
                );
                exit();
            }
        } catch(Exception $e){
            CakeLog::error("[AlarmProgram::getExceptionDetails] {$e->getMessage()}");
            $this->responseWithCode('InternalServerError', $e->getMessage());
        }
    }
    
    /**
     * Ottiene la lista delle zone
     * REST url: /Configuration/Users.json (POST)
     */
    public function putUserList(){
        try {
            $data = $this->request->input('json_decode');
            $user_list = array();
            
            foreach($data->users as $user) {
                $old_user = $this->ModHorusUser->findByName($user->name);
                $old_user['ModHorusUser']['enable'] = ($user->enable_status=="true"? 1 : 0);
                $this->ModHorusUser->save($old_user);
                $user_list[] = $user->name;
            }
            $this->ModHorusUser->deleteAll(array(
                "NOT" => array( "ModHorusUser.name" => $user_list)
            ), true);
            
            if ($this->isREST()) {
                $this->set(array(
                    'response' => array(
                        'message' => 'ok',
                        'agent_upload_results' => true
                    ),
                    '_serialize' => array('response')
                ));
            } else {
                echo json_encode(array(
                    'response' => array(
                        'message' => 'ok',
                        'agent_upload_results' => true
                )));
                exit();
            }
        } catch (Exception $ex) {
            CakeLog::error("[AlarmProgram::getExceptionDetails] {$ex->getMessage()}");
            $this->responseWithCode('InternalServerError', $ex->getMessage());
        }
    }

    
    /**
     * Autenticazione prima di mandare un comando (parental control)
     * @param string $level livello di permesso
     * @param string $passwd password di permesso
     * @throws Exception lancia l'eccezione in caso di non validazione
     */
    public function externalAuthLevel($level, $passwd = NULL){
       if ($this->isREST()) {
           try {
            $passwd = Sanitize::clean($passwd);
            $level = Sanitize::clean($level);
            
            $select = array($level);
            if($level != 'Alarm_OFF' && $level != "Administration") {
                array_push($select, 'Alarm_OFF');
            }
            if($passwd == NULL) {
                throw new Exception('empty password');
            } else {
                $user = $this->ModHorusUserHasExtAvailable->find('first', array(
                    "conditions" => array(
                       "ModHorusUser.password" => $passwd, //AuthComponent::password($passwd),
                       "ModHorusUser.enable" => 1,
                       "ModHorusExtAvailable.name" => $select
                    ),
                    "recursive" => 2
                ));
            }
            if(empty($user)) {
                throw new Exception('Wrong password or not available');
            }
            $auth_user = $this->ModHorusConfiguration->findByCode("USER_AUTH");
            $auth_user["ModHorusConfiguration"]["value"] = $user["ModHorusUser"]["name"];
            $this->ModHorusConfiguration->save($auth_user);
            
            //Utente non valido
            $this->set(array(
                'response' => array(
                     'message' => 'ok',
                     'user' => $user['ModHorusUser']['name']
                ),
                '_serialize' => array('response')
            ));
           } catch(Exception $e){
               CakeLog::error("[Authmanager::authenticate] {$e->getMessage()}");
               $this->responseWithCode('Forbidden', 'Authentication credentials missing.');
           }
       }
    }
    
    /**
     * Restituisce, se esiste, il dettaglio del utente altrimenti restituisce
     * l'elenco vuoto dei permessi.
     * @param string $user_name nome utente
     */
    public function getUser($user_name){
        try {
            $user = $this->ModHorusUser->findByName($user_name);
            $levels = $this->ModHorusExtAvailable->find('all');
            $settings = false;
            $level_list = array();
                
            if(empty($user)) {
                $password = $this->ModHorusConfiguration->findByCode("TEMP_PASSWORD");
                if(!empty($password)) {
                   $this->ModHorusConfiguration->deleteAll(array(
                       "code" => "TEMP_PASSWORD"
                   ));
                   $password = $password["ModHorusConfiguration"]["value"];
                } else {
                   $password = ""; 
                }
                foreach ($levels as $level) {
                   if($level["ModHorusExtAvailable"]["name"] == "Administration")
                       continue;
                   
                   $level_list[] = array (
                       "name" => __($level["ModHorusExtAvailable"]["label"]),
                       "selection_status" => false
                   );
                }
            } else {
                $password = $user["ModHorusUser"]["password"];
                
                //Creo la lista per i sensori
                $levels_selected = $this->ModHorusUserHasExtAvailable->find('list', array(
                    "fields" => array('mod_horus_ext_available_id', 'mod_horus_user_id'),
                    "conditions" => array("mod_horus_user_id" => $user["ModHorusUser"]["id"]),
                ));
                
                foreach ($levels as $level) {
                   if($level["ModHorusExtAvailable"]["name"] == "Administration") {
                       $settings = isset($levels_selected[$level["ModHorusExtAvailable"]["id"]])?true:false;
                       continue;
                   }
                   $level_list[] = array (
                       "name" => __($level["ModHorusExtAvailable"]["label"]),
                       "selection_status" => isset($levels_selected[$level["ModHorusExtAvailable"]["id"]])?true:false
                   );
                }
            }
            if ($this->isREST()) {
                $this->set(array(
                    'response' => array(
                        'message' => 'ok',
                        'password' => $password,
                        'administration_access' => $settings,
                        'alarm_operations' => $level_list
                    ),
                    '_serialize' => array('response')
                ));
            } else {
                echo json_encode(array(
                    'response' => array(
                        'message' => 'ok',
                        'password' => $password,
                        'settings' => $settings,
                        'alarm_operations' => $level_list
                )));
                exit();
            }
        } catch (Exception $ex) {
            CakeLog::error("[AlarmProgram::getZone] {$ex}");
            $this->responseWithCode('InternalServerError', $ex->getMessage());
        }
    }
    
    /**
     * Imposta l'utente con la definizione dei nomi e dei permessi
     * @param string $user_name nome utente
     */
    public function putUser($user_name){
        $datasource = $this->ModHorusUser->getDataSource();
        $datasource->begin();
        try {
            $data = $this->request->input('json_decode');
            CakeLog::error(print_r($data, true));
            
            $univocal_passwd = $this->ModHorusUser->findByPassword($data->password);
            $univocal_passwd = empty($univocal_passwd)?true:false;
            
            if(empty($data->alarm_operations)) {
                //Controllo se è il nuovo utente è disponibile, stessa cosa per la password
                $this->ModHorusConfiguration->create();
                $this->ModHorusConfiguration->save(array(
                   "code"  => "TEMP_PASSWORD",
                   "value" => $data->password
                ));
                
                $univocal_user = $this->ModHorusUser->findByName($user_name);
                $univocal_user = empty($univocal_user)?true:false;
                
            } else {
                $univocal_user = true;
                $user = $this->ModHorusUser->findByName($user_name);
                
                if(empty($user)){
                    $this->ModHorusUser->create();
                    $this->ModHorusUser->save(array(
                        'name' => $user_name,
                        'enable' => 0,
                        'password' => $data->password
                    ));
                    $user_id = $this->ModHorusUser->id;
                } else {
		    $user_id = $user["ModHorusUser"]["id"];
                    $passwd = $this->ModHorusUser->findByPassword($data->password);
                    if(empty($passwd)){
                        $user["ModHorusUser"]["password"] = $data->password;
                        $this->ModHorusUser->save($user);
		    } 
                }
                $this->ModHorusUserHasExtAvailable->deleteAll(array(
                    "ModHorusUserHasExtAvailable.mod_horus_user_id" => $user_id
                ));
                
                //Aggiungo il settings tra le azioni
                $data->alarm_operations[] = (object)array(
                    "name" => "Administration",
                    "selection_status" => $data->administration_access =="true"?true:false
                );
                
                //Inserimento delle associazioni tra sensori e zone
                $levels = $this->ModHorusExtAvailable->find('list', array(
                    "fields" => array('ModHorusExtAvailable.label', 'ModHorusExtAvailable.id'),
                    "recursive" => 2
                ));
                
                foreach($data->alarm_operations as $operation) {
                   if($operation->selection_status == "true") {
                       $this->ModHorusUserHasExtAvailable->create();
                       //Se e' una lingua tradotta
                       if($this->table_translate != NULL) {
                            $this->ModHorusUserHasExtAvailable->save(array(
                                 "mod_horus_user_id" => $user_id,
                                 "mod_horus_ext_available_id" => $levels[$this->table_translate[$operation->name]]
                            ));
                       } else {
                           $this->ModHorusUserHasExtAvailable->save(array(
                                 "mod_horus_user_id" => $user_id,
                                 "mod_horus_ext_available_id" => $levels[$operation->name]
                            ));
                       }
                   }
                }
                
            }
            $datasource->commit();
            if ($this->isREST()) {
                $this->set(array(
                    'response' => array(
                        'message' => 'ok',
                        'agent_upload_results' => true,
                        'univocal_user_name' => $univocal_user,
                        'univocal_password' => $univocal_passwd
                    ),
                    '_serialize' => array('response')
                ));
            } else {
                echo json_encode(array(
                    'response' => array(
                        'message' => 'ok',
                        'agent_upload_results' => true,
                        'univocal_user_name' => $univocal_user,
                        'univocal_password' => $univocal_passwd
                )));
                exit();
            }
        } catch (Exception $ex) {
            $datasource->rollback();
            CakeLog::error("[AlarmProgram::putZone] {$ex}");
            $this->responseWithCode('InternalServerError', $ex->getMessage());
        }
    }
}

?>
