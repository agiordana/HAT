<?php

/*
 * Copyright 2014 Penta Dynamic Solutions S.r.l.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Description of ModHorusZone
 *
 * @author Alessandro Gallina <alessandro.gallina88@hotmail.it>
 */
class ModHorusUserHasExtAvailable extends AppModel {
    
    var $actsAs = array('Containable');
    
    public $belongsTo = array(
        'ModHorusUser' => array(
            'className' => 'ModHorusUser',
            'foreignKey' => 'mod_horus_user_id'
        ),
        'ModHorusExtAvailable' => array(
            'className' => 'ModHorusExtAvailable',
            'foreignKey' => 'mod_horus_ext_available_id'
        )
    );
}