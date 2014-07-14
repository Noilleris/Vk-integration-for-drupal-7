jQuery(document).ready(function() {
    jQuery('.description .gettoken').click(function(){
        var
            app_id = jQuery('#edit-vkxp-app-id').val();
        if (app_id) {
            var
                url = 'https://oauth.vk.com/authorize?client_id='+jQuery('#edit-vkxp-app-id').val()+'&scope=wall%2Cgroups%2Cphotos%2Coffline%2Cmessages&redirect_uri=http%3A//oauth.vk.com/blank.html&response_type=code';
            window.open(url);
        } else {
            alert('Заполните Application ID');
        }
        return false;
    })
})