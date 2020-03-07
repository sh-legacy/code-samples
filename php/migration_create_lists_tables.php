<?php

use Illuminate\Support\Facades\Schema;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Database\Migrations\Migration;

class CreateListsTables extends Migration
{
    /**
     * Run the migrations.
     *
     * @return void
     */
    public function up()
    {
        Schema::create('lists', function (Blueprint $table) {
            $table->increments('id');
            $table->unsignedInteger('server_id');
            $table->string('name');
            //$table->text('layout');
            $table->timestamps();

            $table->unique(['name', 'server_id']);
            $table->foreign('server_id')->references('id')->on('servers')->onUpdate('cascade')->onDelete('cascade');
        });

        Schema::create('list_versions', function (Blueprint $table) {
            $table->increments('id');
            $table->unsignedInteger('list_id');
            $table->string('name');
            $table->string('display_name')->nullable();
            $table->text('layout')->nullable();
            $table->timestamps();

            $table->unique(['name', 'list_id']);
            $table->foreign('list_id')->references('id')->on('lists')->onUpdate('cascade')->onDelete('cascade');
        });

        Schema::create('list_texts', function (Blueprint $table) {
            $table->increments('id');
            $table->unsignedInteger('list_id');
            $table->unsignedInteger('list_version_id');
            $table->text('text');
            $table->timestamps();

            $table->foreign('list_id')->references('id')->on('lists')->onUpdate('cascade')->onDelete('cascade');
            $table->foreign('list_version_id')->references('id')->on('list_versions')->onUpdate('cascade')->onDelete('cascade');
        });

        Schema::create('category_list', function (Blueprint $table) {
            $table->unsignedInteger('category_id');
            $table->unsignedInteger('list_id');
            $table->unsignedInteger('position');

            $table->foreign('category_id')->references('id')->on('categories')->onUpdate('cascade')->onDelete('cascade');
            $table->foreign('list_id')->references('id')->on('lists')->onUpdate('cascade')->onDelete('cascade');
        });

        Schema::create('list_user', function (Blueprint $table) {
            $table->unsignedInteger('list_id');
            $table->unsignedInteger('user_id');
            $table->unsignedInteger('category_id');
            $table->unsignedInteger('position'); // global position

            $table->foreign('list_id')->references('id')->on('lists')->onUpdate('cascade')->onDelete('cascade');
            $table->foreign('user_id')->references('id')->on('users')->onUpdate('cascade')->onDelete('cascade');
            $table->foreign('category_id')->references('id')->on('categories')->onUpdate('cascade');

            $table->unique(['list_id', 'user_id', 'position']);
        });
    }

    /**
     * Reverse the migrations.
     *
     * @return void
     */
    public function down()
    {
        Schema::dropIfExists('list_user');
        Schema::dropIfExists('category_list');
        Schema::dropIfExists('list_texts');
        Schema::dropIfExists('list_versions');
        Schema::dropIfExists('lists');
    }
}
