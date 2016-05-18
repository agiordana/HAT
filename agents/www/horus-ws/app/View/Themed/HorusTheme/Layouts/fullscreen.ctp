<?php echo $this->Html->docType('html5'); ?>
<html>
    <head>
        <?php echo $this->Html->charset(); ?>
        <title>Horus Web | Penta Dynamic Solutions s.r.l.</title>
        <meta name="apple-mobile-web-app-capable" content="yes" />
        <?php
        echo $this->Html->meta(array(
            'type' => 'icon',
            'rel' => 'apple-touch-icon',
            'href' => $this->webroot . 'theme/HorusTheme/img/iphone.png'));

        echo $this->Html->meta('icon');

        echo $this->Html->css('bootstrap');
        echo $this->Html->css('style');
        echo $this->Html->css('mobiscroll/mobiscroll.core');
        echo $this->Html->css('mobiscroll/mobiscroll.wp');
        echo $this->Html->css('mobiscroll/mobiscroll.animation');
        
        //echo $this->Html->script('jquery-1.9.1.min');
        echo $this->Html->script('jquery-2.0.3.min');
        echo $this->Html->script('bootstrap.min');
        echo $this->Html->script('mobiscroll/mobiscroll.core');
        echo $this->Html->script('mobiscroll/mobiscroll.datetime');
        echo $this->Html->script('mobiscroll/mobiscroll.select');
        echo $this->Html->script('builder.js');
        echo $this->Html->script('serialize-json.jquery.js');

        echo $this->fetch('meta');
        echo $this->fetch('css');
        echo $this->fetch('script');
        ?>
    </head>
    <body>
        <script type="text/javascript">
	    var auth_url = '<?php echo $this->Html->Url("/Authmanager/login/1") ?>';
            $(function() {
                scroll_init();
     
                $(document).ajaxError(function(e, xhr, settings, exception) {
                    if(xhr.status==401) {
                        window.location.reload();
                    }
                });
                
                $(".btn-redirect").click(function() {
                    window.location = "<?php echo $this->webroot; ?>" + $(this).data("url");
                });
                
                //Richiede autenticazione
                $("[data-auth]").one('click', function(e) {
                    if ($(this).data('auth') == '0') {
                        $('.modal').modal('hide');
                        $.ajax({
                            url: auth_url,
                            data: {page_url: "<?php echo $this->webroot; ?>"+$(this).data('url')},
                            success: function(msg) {
                                $("#AuthModal").html(msg);
                                $("#AuthModal").modal('show');
                            }
                        });
                        $(this).data('auth', '1');
                        e.stopImmediatePropagation();
                        return false;
                    } else {
                        $(this).data('auth', '0');
                        return true;
                    }
                });
            });
        </script>
        <div class="row-fluid" id="notice">
           <?php echo $this->Session->flash(); ?>
        </div>
        <?php echo $this->fetch('content'); ?>

    </body>
</html>
<!-- Modal -->
<div id="AuthModal" class="modal hide fade" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true"></div>
