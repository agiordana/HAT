<?php echo $this->Html->docType('html5'); ?>
<html>
    <head>
        <?php echo $this->Html->charset(); ?>
        <title>Horus Web | Penta Dynamic Solutions s.r.l.</title>
        <?php
        echo $this->Html->meta('icon');
        echo $this->Html->css('bootstrap.min');
        echo $this->Html->css('bootstrap-theme.min');
        echo $this->Html->css('style');

        echo $this->Html->script('jquery-2.1.1.min');
        echo $this->Html->script('bootstrap.min');

        echo $this->fetch('meta');
        echo $this->fetch('css');
        echo $this->fetch('script');
        ?>
    </head>
    <body role="document">
        <nav class="navbar navbar-inverse navbar-fixed-top" role="navigation">
            <div class="container">
                <div class="navbar-header">
                    <a class="navbar-brand" href="#">Horus Web Service</a>
                </div>
                <div id="navbar" class="navbar-collapse collapse">
                    <ul class="nav navbar-nav">
                        <li><a href="#"> <?= __('Api'); ?></a></li>
                        <li><a href="#"> <?= __('About'); ?></a></li>
                    </ul>
                </div>
            </div>
        </nav>
        <div class="container theme-showcase" role="main">
            <div class="jumbotron">
                <center><?= $this->Html->image("panel.png", array("height" => "250px")); ?></center>
            </div>
            <p>
                <?php echo $this->Session->flash(); ?>
            </p>
            <p>
                <?php echo $this->fetch('content'); ?>
            </p>
        </div>
    </body>
</html>