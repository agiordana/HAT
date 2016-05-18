<?php
/**
 * Permette l'importazione/esportazione dei contenuti necessari alla
 * impostazione della applicazione esterna
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
class ExternalConfigController extends AppController  {
    
    //Aggiunge il supporte del filesystem
    public function beforeFilter() {
        parent::beforeFilter();
        
        App::uses('Folder', 'Utility');
        App::uses('File', 'Utility');
    }
    
    /**
     * Permette l'upload dei file di configurazione per l'app di android
     * REST url: /effective_pch_storage/<folder> (POST)
     * @param string $folder path relativa del file da upload 
     */
    public function upload($folder) {

        $upload = FALSE;
        try {
            if(empty($this->data)) {
                $upload = FALSE;
            } else {            
                $ext = $this->request->params["ext"];
                $file_path = WWW_ROOT . "files" . DS . "external_storage" . DS . $folder . "." . $ext;
                $directory_path = pathinfo($file_path);
                $dir = new Folder($directory_path["dirname"], true);

                //Trova tutti i file e li elimina
                if ($this->data['Configuration']['clean'] == "true") {
                    $files = $dir->read();
                    foreach ($files[1] as $file) {
                        $fp = new File($directory_path["dirname"] . DS . $file);
                        $fp->delete();
                    }
                }
                $file = new File($this->data['Configuration']['file']['tmp_name']);
                $file->copy($file_path, true);

                $upload = TRUE;
            }
        } catch (Exception $e) {
            CakeLog::error("[ExternalConfig::upload] {$e->getMessage()}");
            $upload = FALSE;
        }
        echo json_encode(array(
            'response' => array(
                'message' => 'ok',
                'upload' => $upload,
            )
        ));
        exit();
    }

    /**
     * Restituisce l'elenco dei file all interno della cartella specificata
     * REST url: /effective_pch_storage/<folder> (GET)
     * @param string $folder path relativa che si vuole visualizzare l'elenco
     */
    public function file_list($folder) {
        
        $directory_path = WWW_ROOT . "files" . DS . "external_storage" . DS . $folder;
        $dir = new Folder($directory_path, true);
        $files = $dir->read();
        $files_list = array();
        foreach ($files[1] as $file) {
            $files_list[] = array("name" => $file);
        }
        echo json_encode(array(
            'response' => array(
                'message' => 'ok',
                'files' => $files_list
            )
        ));
        exit();
    }

    /**
     * Esegue la rimozione fisica del file specificato
     * REST url: /effective_pch_storage/<file> (DELETE)
     * @param string $file path relativa del file che si vuole rimuovere
     */
    public function delete($file) {
        $start = strlen("/horus-ws/effective_pch_storage");
        $real_file = substr($this->here, $start);
        $directory_path = WWW_ROOT . "files" . DS . "external_storage" . DS;
        $file = new File($directory_path . $real_file);
        $file->delete();

        echo json_encode(array(
            'response' => array(
                'message' => 'ok',
                'upload' => true,
            )
        ));
        exit();
    }

    /**
     * Effettua il download del file che si vuole scaricare
     * REST url: /Storage/<file> (GET) 
     * @param string $file percorso relativo del file che si vuole scaricare
     * @return File il download del file in binario
     */
    public function download($file) {
        $start = strlen("/horus-ws/Storage");
        $real_file = substr($this->here, $start);
        $directory_path = WWW_ROOT . "files" . DS . "external_storage" . DS;

        $this->response->file($directory_path . $real_file, array(
            'download' => true,
            'name' => basename($real_file),
        ));
        return $this->response;
    }

}
