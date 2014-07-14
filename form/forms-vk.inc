<?php
    function mass_social_add_vk_group($form, &$form_state, $s_network = null) {
        // Application settings.
        if ($s_network) {

            $form['id'] = array(
                '#type' => 'hidden',
                '#default_value' =>$s_network['id']
            );

            $form['network_name'] = array(
                '#type' => 'hidden',
                '#default_value' =>$s_network['network_name']
            );
        }

        $form['vkxp_application'] = array(
            '#type' => 'fieldset',
            '#title' => t('Application settings'),
            '#collapsible' => TRUE,
            '#description' => t('Note: You have to <a href="@url" target="_blank">create</a> a <strong>standalone</strong> application to make this module work.',
                array('@url' => 'http://vk.com/editapp?act=create')),
        );

        $form['vkxp_application']['vkxp_app_id'] = array(
            '#type' => 'textfield',
            '#title' => t('Application ID'),
            '#description' => t('ID of vk application.'),
            '#required' => TRUE,
            '#element_validate' => array('element_validate_integer_positive'),
            '#default_value' =>$s_network?$s_network["data"]['vkxp_app_id']:''
        );

        $form['vkxp_application']['vkxp_app_secret'] = array(
            '#type' => 'textfield',
            '#title' => t('Application secret code'),
            '#description' => t('Secret code of vk application.'),
            '#required' => TRUE,
            '#default_value' =>$s_network?$s_network["data"]['vkxp_app_secret']:''
        );

        $form['vkxp_application']['vkxp_code'] = array(
            '#type' => 'textfield',
            '#title' => t('Access code'),
            '#description' => t('Get access code <a class="gettoken">here</a>'),
            '#required' => TRUE,
            '#default_value' =>$s_network?$s_network["data"]['vkxp_code']:''
        );

        $form['vkxp_application']['vkxp_token'] = array(
            '#type' => 'hidden',
            '#required' => TRUE,
            '#default_value' =>$s_network?$s_network["data"]['vkxp_token']:''
        );

        // Owner settings.
        $form['vkxp_owner'] = array(
            '#type' => 'fieldset',
            '#title' => t('Owner settings'),
            '#description' => t('Set user or group to which data will be transferred.'),
            '#collapsible' => TRUE
        );

        $form['vkxp_owner']['vkxp_owner_id'] = array(
            '#type' => 'textfield',
            '#title' => t('Owner ID'),
            '#description' => t('User ID or Group ID.'),
            '#required' => TRUE,
            '#element_validate' => array('element_validate_integer_positive'),
            '#default_value' =>$s_network?$s_network["data"]['vkxp_owner_id']:''
        );

        $form['vkxp_owner']['vkxp_wall_owner'] = array(
            '#type' => 'select',
            '#title' => t('Select owner type'),
            '#description' => t('Who owns the ID above.'),
            '#options' => array(
                'group' => t('Group'),
                'user' => t('User'),
            ),
            '#default_value' =>$s_network?$s_network["data"]['vkxp_wall_owner']:''
        );

        // Additional settings.
        $form['vkxp_additional'] = array(
            '#type' => 'fieldset',
            '#title' => t('Additional settings'),
            '#collapsible' => TRUE,
        );

        $form['vkxp_additional']['vkxp_official'] = array(
            '#type' => 'checkbox',
            '#title' => t('Write from group name'),
            '#description' => t('Check this if you want to post messages from group name.'),
            '#default_value' =>$s_network?$s_network["data"]['vkxp_official']:''
        );

        $form['vkxp_additional']['vkxp_add_link'] = array(
            '#type' => 'checkbox',
            '#title' => t('Add link on wall to posted page'),
            '#description' => t('Check this if you want to post node url on vk wall.'),
            '#default_value' =>$s_network?$s_network["data"]['vkxp_add_link']:''
        );

        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Save')
        );

        return $form;
    }

    function mass_social_add_vk_group_submit($form, &$form_state) {
        $params = array();
        $params['client_id']     = $form['vkxp_application']['vkxp_app_id']["#value"];
        $params['client_secret'] = $form['vkxp_application']['vkxp_app_secret']["#value"];
        $params['code']          = $form['vkxp_application']['vkxp_code']["#value"];
        $params['redirect_uri']  = 'http://oauth.vk.com/blank.html';

        $data = mass_social_query('', $params, VKXP_ACCESS_TOKEN_URI);
        $token = '';

        // Access token was recieved.
        if (!empty($data['access_token'])) {
            $message = array(
                'text'     => t('Access token was successfully recieved.'),
                'severity' => 'status',
            );
            _vkxp_watchdog($message);

            $token = $data['access_token'];
        }
        // Access token was not recieved.
        elseif (isset($data['error']) && isset($data['error_description'])) {
            $message = array(
                'text'     => t('Access token was not recieved. Reason: %error', array('%error' => check_plain($data['error_description']))),
                'severity' => 'error',
            );
            _vkxp_watchdog($message);

            // Remove variable if user not allowed to get a new access token.
            $token = 0;
        }

        $array_blob = array(
            'vkxp_app_id' => $form['vkxp_application']['vkxp_app_id']["#value"],
            'vkxp_app_secret' => $form['vkxp_application']['vkxp_app_secret']["#value"],
            'vkxp_code' => $form['vkxp_application']['vkxp_code']["#value"],
            'vkxp_token' => $token,
            'vkxp_owner_id' => $form['vkxp_owner']['vkxp_owner_id']["#value"],
            'vkxp_wall_owner' => $form['vkxp_owner']['vkxp_wall_owner']["#value"],
            'vkxp_official' => $form['vkxp_additional']['vkxp_official']["#value"],
            'vkxp_add_link' => $form['vkxp_additional']['vkxp_add_link']["#value"],
        );

        if ($form['id']["#value"]) {
            $query = db_update('mass_social');
            $query->fields(array(
                'data' => serialize($array_blob),
            ));
            $query->condition('id', $form['id']["#value"]);
            $num_updated = $query->execute();
        } else {
            db_insert('mass_social')
                ->fields(array(
                    'network_name' => 'VK'.$form['vkxp_application']['vkxp_app_id']["#value"],
                    'data' => serialize($array_blob),
                ))
                ->execute();
        }
        $form_state['redirect'] = 'admin/config/user-interface/mass_social/list';
        return $form;
    }