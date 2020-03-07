<?php

namespace App\Http\Controllers\API\v1;

use App\Http\Controllers\Controller;
use App\Http\Requests\v1\Categories\DestroyRequest;
use App\Http\Requests\v1\Categories\IndexRequest;
use App\Http\Requests\v1\Categories\ShowRequest;
use App\Http\Requests\v1\Categories\StoreRequest;
use App\Http\Requests\v1\Categories\UpdateRequest;
use App\Models\Category;
use App\Models\Property;
use App\Models\Role;
use App\Models\Server;
use Illuminate\Http\Request;
use App\Http\Resources\v1\Category as CategoryResource;
use Illuminate\Support\Facades\Log;

class CategoryController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @param IndexRequest $request
     * @param Server $server
     * @return \Illuminate\Http\Resources\Json\AnonymousResourceCollection
     */
    public function index(IndexRequest $request, Server $server)
    {
        return CategoryResource::collection(Category::where('server_id', $server->id)->get());
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param StoreRequest $request
     * @param Server $server
     * @return CategoryResource
     */
    public function store(StoreRequest $request, Server $server)
    {
        $category = Category::create(array_merge($request->all(), ['server_id' => $server->id]));

        $properties = Property::whereIn('name', $request->all()['properties']);

        $category->properties()->sync($properties->pluck('id')->all());
        $category->defaultForRoles()->sync($request->all()['default_for_roles']);

        return new CategoryResource($category);
    }

    /**
     * Display the specified resource.
     *
     * @param ShowRequest $request
     * @param Server $server
     * @param Category $category
     * @return CategoryResource
     */
    public function show(ShowRequest $request, Server $server, Category $category)
    {
        return new CategoryResource($category);
    }

    /**
     * Update the specified resource in storage.
     *
     * @param UpdateRequest $request
     * @param Server $server
     * @param Category $category
     * @return CategoryResource
     */
    public function update(UpdateRequest $request, Server $server, Category $category)
    {
        //Log::debug(print_r($request->all()['default_for_roles'], true));
        $category->update(array_merge($request->all(), ['server_id' => $server->id]));
        $properties = Property::whereIn('name', $request->all()['properties']);
        $category->properties()->sync($properties->pluck('id')->all());
        $category->defaultForRoles()->sync($request->all()['default_for_roles']);

        return new CategoryResource($category);
    }

    /**
     * Remove the specified resource from storage.
     *
     * @param DestroyRequest $request
     * @param Server $server
     * @param Category $category
     * @return \Illuminate\Http\Response
     * @throws \Exception
     */
    public function destroy(DestroyRequest $request, Server $server, Category $category)
    {
        $category->delete();
        return response()->deleted();
    }
}
