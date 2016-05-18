<?php

App::uses('AppModel', 'Model');

/**
 * ModHorusExtAvailable Model
 *
 * @property ModHorusExtAvailable $ModHorusExtAvailable
 */
class ModHorusExtAvailable extends AppModel {
    /**
     * Validation rules
     *
     * @var array
     */
    public $validate = array();
    
    public $hasAndBelongsToMany = array(
        'ModHorusUser' => array(
            'className' => 'ModHorusUser',
            'joinTable' => 'mod_horus_user_has_ext_availables',
            'foreignKey' => 'mod_horus_ext_available_id',
            'associationForeignKey' => 'mod_horus_user_id',
            'unique' => 'keepExisting'
        )
    );
}
