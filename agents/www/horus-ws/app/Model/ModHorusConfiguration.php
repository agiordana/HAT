<?php
App::uses('AppModel', 'Model');
/**
 * ModHorusConfiguration Model
 *
 */
class ModHorusConfiguration extends AppModel {

/**
 * Primary key field
 *
 * @var string
 */
	public $primaryKey = 'code';

/**
 * Validation rules
 *
 * @var array
 */
	public $validate = array(
		'value' => array(
			'notempty' => array(
				'rule' => array('notempty'),
				//'message' => 'Your custom message here',
				//'allowEmpty' => false,
				//'required' => false,
				//'last' => false, // Stop validation after this rule
				//'on' => 'create', // Limit validation to 'create' or 'update' operations
			),
		),
	);
}
