<?php

App::uses('AppModel', 'Model');

/**
 * ModHorusUser Model
 *
 * @property ModHorusUserLoginLog $ModHorusUserLoginLog
 */
class ModHorusUser extends AppModel {

    /**
     * Validation rules
     *
     * @var array
     */
    public $validate = array();

    //The Associations below have been created with all possible keys, those that are not needed can be removed
    public $hasAndBelongsToMany = array(
        'ModHorusExtAvailable' => array(
            'className' => 'ModHorusExtAvailable',
            'joinTable' => 'mod_horus_user_has_ext_availables',
            'foreignKey' => 'mod_horus_user_id',
            'associationForeignKey' => 'mod_horus_ext_available_id',
            'unique' => 'keepExisting'
        )
    );

    public function user_permission($id){
        $perms = $this->ModHorusPermission->find('all', array(
            "conditions" => array("mod_horus_user_id" => $id),
            "fields" => "task"
        ));
        $list = array();
        foreach($perms as $perm) {
            $list[] = $perm["ModHorusPermission"]["task"];
        }
        return $list;
    }
    
    public function beforeSave($options = array()) {
        if (isset($this->data[$this->alias]['password'])) {
            //$this->data[$this->alias]['password'] = AuthComponent::password($this->data[$this->alias]['password']);
        }
        return true;
    }

}
