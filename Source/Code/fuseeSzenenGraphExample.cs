using System;
using System.Collections.Generic;
using System.Diagnostics;
using Fusee.Engine;
using Fusee.Math;
using Fusee.Serialization;
using Fusee.Engine.SimpleScene;

namespace Examples.SimpleSceneContainer
{
    [FuseeApplication(Name = "SimpleSceneGraph Example", Description = "A very simple example for the new SceneGraph system.")]
    public class Simple : RenderCanvas
    {
        // We only need one ref to a container for the root container of the scene.
        private SceneContainer sceneContainer;

        // We have a scene visitor with which we can travel through our scene.
        private SceneRenderer sceneVisitorRender;

        // is called on startup
        public override void Init()
        {
            RC.ClearColor = new float4(0, 0, 0, 1);

            // Init the SceneContainer.
            sceneContainer = new SceneContainer();
            sceneContainer.Header = new SceneHeader();
            sceneContainer.Children = new List<SceneNodeContainer>();

            // Fill the headers information.
            sceneContainer.Header.CreatedBy = "Fusee Team";
            sceneContainer.Header.CreationDate = "Year 2015";
            sceneContainer.Header.Generator = "Some Generator";
            sceneContainer.Header.Version = 1;

            // Create a node as our first object node in the scene.
            SceneNodeContainer node1 = new SceneNodeContainer();
            node1.Name = "firstObjectNode";
            node1.Components = new List<SceneComponentContainer>();

            // Add another node to the scene which is in fact empty.
            SceneNodeContainer node2 = new SceneNodeContainer();
            node2.Name = "secondObjectNode";
            node2.Components = new List<SceneComponentContainer>();

            // Loading a mesh from a file to a MeshComponent object.
            MeshComponent mesh1 = new MeshComponent();
            LoadMesh(@"Assets/Cube.obj.model", out mesh1);
            
            // Creating a transform component to manipulate transformations.
            TransformComponent transf1 = new TransformComponent();
            //transf1.Scale = new float3() { x = 1f, y = 1, z = 1};
            //transf1.Translation = new float3() { x = 1f, y = 1, z = 1 };
            //transf1.Rotation = new float3() { x = 1f, y = 1, z = 1 };

            // Material Component
            MaterialComponent mat1 = new MaterialComponent();
            mat1.Diffuse = new MatChannelContainer();
            mat1.Diffuse.Color = new float3(1f, 0, 0);

            // Add the components to the first node.
            node1.Components.Add(mesh1);
            node1.Components.Add(transf1);
            node1.Components.Add(mat1);

            // Add another node to the node1s children.
            SceneNodeContainer node1child = new SceneNodeContainer();
            node1child.Name = "node1secondObjectNode";
            node1child.Components = new List<SceneComponentContainer>();

            node1.Children = new List<SceneNodeContainer>();
            node1.Children.Add(node1child);

            // Add the node with all the components to the scene.
            sceneContainer.Children.Add(node1);
            sceneContainer.Children.Add(node2);
            
            // Create the renderer and add the scene Container to it. Also append the render context.
            sceneVisitorRender = new SceneRenderer(sceneContainer, @"Assets/Cube.obj.model");
            sceneVisitorRender.SetContext(RC);
        }

        // is called once a frame
        public override void RenderAFrame()
        {
            // Clear the buffers
            RC.Clear(ClearFlags.Color | ClearFlags.Depth);
            
            // Render all the children in the sceneNode.
            sceneVisitorRender.Render(RC);

            // swap buffers
            Present();
        }

        // is called when the window was resized
        public override void Resize()
        {
            RC.Viewport(0, 0, Width, Height);

            var aspectRatio = Width/(float) Height;
            var projection = float4x4.CreatePerspectiveFieldOfView(MathHelper.PiOver4, aspectRatio, 1, 5000);
            RC.Projection = projection;
        }

        public static void Main()
        {
            var app = new Simple();
            app.Run();
        }

        /// <summary>
        /// Can load a meshs values to a mesh component object.
        /// This method is purely for convencience.
        /// </summary>
        /// <param name="path"></param>
        /// <param name="m1"></param>
        /// <returns>Mesh</returns>
        public Mesh LoadMesh(String path, out MeshComponent m1)
        {
            MeshComponent m = new MeshComponent();
            Mesh mesh = MeshReader.LoadMesh(path);
            m.Vertices = mesh.Vertices;
            m.Triangles = mesh.Triangles;
            m.Normals = mesh.Normals;
            m.UVs = mesh.UVs;
            // TODO: Bounding box.
            //m1.BoundingBox = mesh;

            m1 = m;
            return mesh;
        }
    }
}