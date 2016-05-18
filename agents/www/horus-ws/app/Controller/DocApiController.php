<?php
/**
 * Controllo della documentazione delle Web Service
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

class DocApiController extends AppController {
    public function beforeFilter() {
        parent::beforeFilter();
        $this->Auth->allow();
    }

    public function index() {
        
    }
}
