<?php
    function _mass_social_node_form(&$form) {

        $query = db_select('mass_social', 'ms');
        $query->fields('ms', array('id', 'network_name'));
        $result = $query->execute();
        $options = array();
        foreach ($result as $template) {
            $options[$template->id] = $template->network_name;
        }
        $output = '';

        $form['mass_social'] = array(
            '#type'   => 'fieldset',
            '#title'  => t('Mass social'),
            '#group'  => 'additional_settings',
            '#weight' => -10,
        );

        $form['mass_social']['ms_post_this_node'] = array(
            '#type'          => 'checkboxes',
            '#title'         => t('Post this node to'),
            '#description'   => t('After you submit this node it will be posted to VK.')
        );

        if(count($options) > 0) {
            $form['mass_social']['ms_post_this_node']['#options'] = $options;
        } else {
            $form['mass_social']['ms_post_this_node']['#disabled']    = TRUE;
            $form['mass_social']['ms_post_this_node']['#value']       = 0;
            $form['mass_social']['ms_post_this_node']['#description'] = t("No social networks to post");
        }

        // Disable crosspost if cURL library is not installed on a server.
        if (!function_exists('curl_init')) {
            $form['mass_social']['ms_post_this_node']['#disabled']    = TRUE;
            $form['mass_social']['ms_post_this_node']['#value']       = 0;
            $form['mass_social']['ms_post_this_node']['#description'] = t("You can't crosspost nodes until cURL library is not installed on your server.");
        }


    }

    function _mass_social_process_node_settings_form(&$form) {

        // Do not process node form if it do not contain node type.
        if (empty($form['#node_type']->type)) {
            return;
        }

        $node_type = $form['#node_type']->type;

        $form['mass_social'] = array(
            '#type'   => 'fieldset',
            '#title'  => t('Mass social settings'),
            '#group'  => 'additional_settings',
        );

        // Enable setting.
        $form['mass_social']['mass_social_node_enabled'] = array(
            '#type'          => 'checkbox',
            '#title'         => t('Enable VKontakte CrossPoster'),
            '#default_value' => variable_get('mass_social_node_enabled_' . $node_type),
            '#description'   => t('Enable or disable crosspost for this node type.')
        );

        // Get information about node fields.
        $fields_info = field_info_instances('node', $node_type);

        // Collect all text and image fields.
        $text_fields  = array('title' => t('Title'));
        $image_fields = array(0 => t('- None -'));
        foreach ($fields_info as $field_name => $field_instance) {
            if ($field_instance['widget']['module'] == 'text') {
                $text_fields[$field_name] = $field_instance['label'] . ' (' . $field_name . ')';
            }
            elseif ($field_instance['widget']['module'] == 'image') {
                $image_fields[$field_name] = $field_instance['label'] . ' (' . $field_name . ')';
            }
        }

        // Message.
        $form['mass_social']['mass_social_body'] = array(
            '#type'  => 'fieldset',
            '#title' => t('Message'),
        );

        // Message field.
        $form['mass_social']['mass_social_body']['mass_social_node_message_field'] = array(
            '#type'          => 'select',
            '#title'         => t('Message source'),
            '#description'   => t('Select field from which to take the text to crosspost.'),
            '#options'       => $text_fields,
            '#default_value' => variable_get('mass_social_node_message_field_' . $node_type, 'title'),
        );

        // Message length.
        $form['mass_social']['mass_social_body']['mass_social_node_message_length'] = array(
            '#type'          => 'textfield',
            '#title'         => t('Cut message'),
            '#field_prefix'  => t('Cut message if it has more than'),
            '#field_suffix'  => t('symbols'),
            '#size'          => 4,
            '#default_value' => variable_get('mass_social_node_message_length_' . $node_type, 255),
            '#description'   => t('Leave this field empty to not cut a message.'),
            '#attributes'    => array('style' => 'width:auto;'), // We need to break default Drupal style for this field.
        );

        // Image.
        $form['mass_social']['mass_social_image'] = array(
            '#type'  => 'fieldset',
            '#title' => t('Image'),
        );

        // Image source.
        $form['mass_social']['mass_social_image']['mass_socialnode_image_field'] = array(
            '#type'          => 'select',
            '#title'         => t('Image source'),
            '#description'   => t('Select field from which to take the image to crosspost.'),
            '#options'       => $image_fields,
            '#default_value' => variable_get('mass_social_node_image_field_' . $node_type, 0),
        );

        // Image limit.
        $form['mass_social']['mass_social_image']['mass_social_node_image_limit'] = array(
            '#type'          => 'select',
            '#title'         => t('Images limit'),
            '#description'   => t('Select maximum amount of images to crosspost.'),
            '#options'       => drupal_map_assoc(range(0, 10)),
            '#default_value' => variable_get('mass_social_node_image_limit_' . $node_type, 0),
        );
    }